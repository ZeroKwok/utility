// This file is part of the utility project.
// Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#include "utility/filesystem.h"
#include "filesystem_p.h"

#ifdef OS_POSIX

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <chrono>
#include <filesystem>

/**
 * @brief 包装系统调用，在遇到 EINTR 时自动重试
 * @param f 可调用对象（系统调用或兼容函数）
 * @param args 可变参数（传递给 f）
 * @return 同 f 的返回值类型
 */
template <typename F, typename... Args>
auto retry_on_intr(F&& f, Args&&... args) 
    -> typename std::invoke_result_t<F, Args...> 
{
    using ResultType = typename std::invoke_result_t<F, Args...>;
    while (true) {
        auto ret = std::forward<F>(f)(std::forward<Args>(args)...);
        
        // 检查是否需要重试（仅对返回 int 且 errno == EINTR 的情况）
        if constexpr (std::is_same_v<ResultType, int>) {
            if (ret == -1 && errno == EINTR) {
                continue;
            }
        }
        return ret;
    }
}

template <typename F, typename P>
inline struct stat stat_no_intr(F &&f, P &&p, std::error_code &error) noexcept
{
    struct stat statbuf = {};
    auto r = retry_on_intr(std::forward<F>(f), std::forward<P>(p), &statbuf);
    if (r == -1)
    {
        error = UTILITY_NAMESPACE::fs::MakeSysError(errno);
        return {};
    }
    return std::move(statbuf);
}

namespace UTILITY_NAMESPACE {
namespace fs {

fptr open(const path& name, int flag, int mode)
{
    std::error_code ecode;
    const auto& result = open(name, flag, mode, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Can't open the file", name);
    return result;
}

fptr open(const path& name, int flag, int mode, std::error_code& error) noexcept
{
    error.clear();

    if (name.empty()) {
        error = MakeSysError(EINVAL);
        return {};
    }

    try
    {
        // 
        // 在操作完成时, 该函数将打开文件并返回表示文件描述符的非负整数(一个进程文件描述符表中未使用的最低编号).
        // 其他情况返回-1, 并设置errno, 此时不会创建或修改文件;
        //
        // https://linux.die.net/man/2/open

        auto fid = retry_on_intr(::open, name.c_str(), flag, mode);
        if(fid == -1) // error
        {
            error = MakeSysError(errno);
            return {};
        }

        return new _file{fid};
    }
    catch (std::bad_alloc)
    {
        error = MakeSysError(ENOMEM);
    }

    return {};
}

void close(const fptr& file)
{
    std::error_code ecode;
    close(file,  ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to close file");
}

void close(const fptr& file, std::error_code& error) noexcept
{
    error.clear();
    if (file == nullptr)
        return;

    if (retry_on_intr(::fsync, file->fd) == -1)
    {
        error = MakeSysError(errno);
        return;
    }

    // 
    // https://linux.die.net/man/2/close
    if (retry_on_intr(::close, file->fd) == -1)
    {
        error = MakeSysError(errno);
        return;
    }

    delete file;
}

size read(const fptr& file, void* data, size len) 
{
    std::error_code ecode;
    const auto& result = read(file, data, len, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to read file");
    return result;
}

size read(const fptr& file, void* data, size len, std::error_code& error) noexcept 
{
    error.clear();
    if (file == nullptr) {
        error = MakeSysError(EINVAL);
        return 0;
    }

    int64_t result = 0;
    uint64_t readBytes = 0;
    do
    {
        uint64_t wantedBytes = len - readBytes;
        uint64_t chunkSize = 0x20000000;
        if (chunkSize > wantedBytes)
            chunkSize = wantedBytes;
        result = ::read(
            file->fd,
            static_cast<char *>(data) + readBytes,
            static_cast<size_t>(chunkSize));
    } while (result > 0 && (readBytes += result) < len);

    // https://linux.die.net/man/2/read
    //
    // 如果成功，则返回读取的字节数（0表示文件结束），发生错误时，返回-1，并设置errno
    //
    // 返回值小于请求的字节数，不会出错, 这可能发生在到达文件末尾 EOF 或者文件被信号中断。
    // 数据小于请求的字节数, 防止被信号中断因此还需要再次读取, 直到明确到达EOF

    if (result == -1 || (result != 0 && readBytes == 0))
        error = MakeSysError(errno);

    return readBytes;
}

size write(const fptr& file, const void *data, size len) 
{
    std::error_code ecode;
    const auto& result = write(file, data, len, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to write file");
    return result;
}

size write(const fptr& file, const void *data, size len, std::error_code& error) noexcept
{
    error.clear();
    if (file == nullptr) {
        error = MakeSysError(EINVAL);
        return {};
    }

    if (len == 0)
        return 0;

    int64_t result = 0;
    uint64_t writtenBytes = 0;
    do
    {
        uint64_t wantedBytes = len - writtenBytes;
        uint64_t chunkSize = 0x20000000;
        if (chunkSize > wantedBytes)
            chunkSize = wantedBytes;
        result = ::write(
            file->fd,
            static_cast<const char *>(data) + writtenBytes,
            static_cast<size_t>(chunkSize));
    } while (result > 0 && (writtenBytes += result) < len);

    // https://linux.die.net/man/3/write
    //
    // write() returns the number of bytes written, or -1 if an error occurred.

    if (result == -1 || writtenBytes == 0)
        error = MakeSysError(errno);

    return writtenBytes;
}

offset seek(const fptr& file, offset offset, int whence)
{
    std::error_code ecode;
    auto r = seek(file, offset, whence, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to seek the file pointer");
    return r;
}

offset seek(const fptr& file, offset offset, int whence, std::error_code& error) noexcept
{
    error.clear();

    if (file == nullptr) {
        error = MakeSysError(EINVAL);
        return -1;
    }

    // https://linux.die.net/man/2/lseek
    // 
    // lseek() returns the new position of the file pointer, or -1 if an error occurred.

    auto r = retry_on_intr(::lseek, file->fd, offset, whence);
    if (r == -1) {
        error = MakeSysError(errno);
        return -1;
    }

    return r;
}

offset tell(const fptr& file) 
{
    std::error_code ecode;
    const auto& result = tell(file, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file pointer");
    return result;
}

offset tell(const fptr& file, std::error_code& error) noexcept 
{
    error.clear();

    if (file == nullptr) {
        error = MakeSysError(EINVAL);
        return {};
    }

    auto r = retry_on_intr(::lseek, file->fd, 0, SEEK_CUR);
    if (r == -1) {
        error = MakeSysError(errno);
        return -1;
    }

    return r;
}

size file_size(const fptr& file) {
    std::error_code ecode;
    const auto& result = file_size(file, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file size");
    return result;
}

size file_size(const fptr& file, std::error_code& error) noexcept 
{
    error.clear();
    if (file == nullptr) {
        error = MakeSysError(EINVAL);
        return {};
    }

    auto statbuf = stat_no_intr(::fstat, file->fd, error);
    if (error)
        return {};
    return statbuf.st_size;
}

ftime file_time(const fptr& file) 
{
    std::error_code ecode;
    const auto& result = file_time(file, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file time");
    return result;
}

inline file_time_type from_time_t(std::time_t t)
{
#if COMPILER_GCC && GCC_VERSION >= 130000
    return std::chrono::clock_cast<file_time_type::clock>(
        std::chrono::system_clock::from_time_t(t));
#else
    auto sys_time = std::chrono::system_clock::from_time_t(t);
    auto sys_dur = sys_time.time_since_epoch();
    auto file_dur = std::chrono::duration_cast<file_time_type::duration>(sys_dur);
    return file_time_type(file_dur);
#endif
}

inline std::time_t to_time_t(const file_time_type& t)
{
#if COMPILER_GCC && GCC_VERSION >= 130000
    return std::chrono::system_clock::to_time_t(
        std::chrono::clock_cast<std::chrono::system_clock>(t));
#else
  auto d = t.time_since_epoch();
  auto s = std::chrono::duration_cast<std::chrono::seconds>(d);
  return s.count();
#endif
}

inline void to_timespec(const file_time_type& t, struct timespec& times)
{
    times.tv_nsec = 0;
    times.tv_sec = to_time_t(t);
}

ftime file_time(const fptr& file, std::error_code& error) noexcept 
{
    error.clear();
    if (file == nullptr) {
        error = MakeSysError(EINVAL);
        return {};
    }

    struct stat statbuf = stat_no_intr(::fstat, file->fd, error);
    if (error)
        return {};

    return ftime{
        from_time_t(statbuf.st_mtime),
        from_time_t(statbuf.st_atime),
#ifdef HAVE_ST_BIRTHTIME
        ft.create_time = statbuf.st_birthtime,
#else
        {},
#endif
        from_time_t(statbuf.st_ctime),
    };
}

ftime file_time(const path& name) 
{
    std::error_code ecode;
    const auto& result = file_time(name, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file time", name);
    return result;
}

ftime file_time(const path& name, std::error_code& error) noexcept 
{
    error.clear();
    if (name.empty()) {
        error = MakeSysError(EINVAL);
        return {};
    }

    struct stat statbuf = stat_no_intr(::stat, name.c_str(), error);
    if (error)
        return {};

    return ftime{
        from_time_t(statbuf.st_mtime),
        from_time_t(statbuf.st_atime),
#ifdef HAVE_ST_BIRTHTIME
        from_time_t(statbuf.st_birthtime),
#else
        {},
#endif
        from_time_t(statbuf.st_ctime),
    };
}

void file_time(const fptr& file, const ftime& time) {
    std::error_code ecode;
    file_time(file, time, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to set the file time");
}

void file_time(const fptr& file, const ftime& time, std::error_code& error) noexcept 
{
    error.clear();
    if (file == nullptr) {
        error = MakeSysError(EINVAL);
        return;
    }

    struct timespec times[2] = {0};
    if (time.last_access)
        to_timespec(*time.last_access, times[0]);
    else
        times[0].tv_nsec = UTIME_OMIT;

    if (time.last_write)
        to_timespec(*time.last_write, times[1]);
    else
        times[1].tv_nsec = UTIME_OMIT;

    // 
    // https://linux.die.net/man/2/utimensat

    if (::futimens(file->fd, times) != 0)  {
        error = MakeSysError(errno);
    }
}

void file_time(const path& name, const ftime& time) 
{
    std::error_code ecode;
    file_time(name, time, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to set the file time", name);
}

void file_time(const path& name, const ftime& time, std::error_code& error) noexcept 
{
    error.clear();
    if (name.empty()) {
        error = MakeSysError(EINVAL);
        return;
    }

    struct timespec times[2] = { 0 };
    if (time.last_access)
        to_timespec(*time.last_access, times[0]);
    if (time.last_write)
        to_timespec(*time.last_write, times[1]);

    // 
    // https://linux.die.net/man/2/utimensat

    if (::utimensat(AT_FDCWD, name.c_str(), times, 0) != 0)  {
        error = MakeSysError(errno);
    }
}

bool is_writable(const path& name) 
{
    std::error_code ecode;
    const auto& result = is_writable(name, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file permissions", name);
    return result;
}

bool is_writable(const path& name, std::error_code& error) noexcept 
{
    error.clear();
    
    if (name.empty()) {
        error = MakeSysError(EINVAL);
        return false;
    }


    return true;
}

} // fs
} // namespace UTILITY_NAMESPACE

#endif // OS_POSIX