// This file is part of the utility project.
// Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#include "utility/filesystem.h"
#include "filesystem_p.h"

#ifdef OS_POSIX

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

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

namespace UTILITY_NAMESPACE {
namespace fs {

fptr open(const path& name, int flag)
{
    std::error_code ecode;
    const auto& result = open(name, flag, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Can't open the file", name);
    return result;
}

fptr open(const path& name, int flag, std::error_code& error) noexcept
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

        auto fid = retry_on_intr(::open, name.string().c_str(), flag, 0644);
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
    if (file == nullptr)
        return;

    if (retry_on_intr(::fsync, file->fid) == -1)
    {
        error = MakeSysError(errno, kFilesystemError);
        return;
    }

    // 
    // https://linux.die.net/man/2/close
    if (retry_on_intr(::close, file->fid) == -1)
    {
        error = MakeSysError(errno, kFilesystemError);
        return;
    }

    delete file;
}

size read(const fptr& file, char* data, int size) 
{
    std::error_code ecode;
    const auto& result = read(file, data, size, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to read file");
    return result;
}

size read(const fptr& file, char* data, int size, std::error_code& error) noexcept 
{
    error.clear();

    if (file == nullptr) {
        error = MakeSysError(EINVAL);
        return 0;
    }

    // https://linux.die.net/man/2/read
    //
    // 如果成功，则返回读取的字节数（0表示文件结束）。
    // 发生错误时，返回-1，并设置errno
    //
    // 返回值小于请求的字节数，不会出错, 这可能发生在到达文件末尾 EOF 或者文件被信号中断。

    int bytes = retry_on_intr(::read, file->fid, data, static_cast<size_t>(size));
    if (bytes == -1) {
        error = MakeSysError(errno);
        return 0;
    }

    // 数据小于请求的字节数, 防止被信号中断因此还需要再次读取, 直到明确到达EOF
    while (bytes > 0 && bytes < size)
    {
        auto r = retry_on_intr(::read, file->fid, data + bytes, static_cast<size_t>(size - bytes));
        if (r == 0)
            return bytes;
        else if (r == -1) {
            error = MakeSysError(errno);
            return bytes;
        }

        bytes += r;
    }

    return bytes;
}

size write(const fptr& file, const char *data, int size) 
{
    std::error_code ecode;
    const auto& result = write(file, data, size, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to write file");
    return result;
}

size write(const fptr& file, const void *data, int size, std::error_code& error) noexcept
{
    error.clear();

    if (file == nullptr) {
        error = MakeSysError(EINVAL);
        return {};
    }

    // TODO
    error = make_error(kNotSupported);

    return bytes;
}

void seek(const fptr& file, size offset, int whence)
{
    std::error_code ecode;
    seek(file, offset, whence, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to seek the file pointer");
}

void seek(const fptr& file, size offset, int whence, std::error_code& error) noexcept
{
    error.clear();

    if (file == nullptr) {
        error = MakeSysError(EINVAL);
        return;
    }

    // TODO
    error = make_error(kNotSupported);
}

size tell(const fptr& file) 
{
    std::error_code ecode;
    const auto& result = tell(file, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file pointer");
    return result;
}

size tell(const fptr& file, std::error_code& error) noexcept 
{
    error.clear();

    if (file == nullptr) {
        error = MakeSysError(EINVAL);
        return {};
    }

    // TODO
    error = make_error(kNotSupported);
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

    // TODO
    error = make_error(kNotSupported);
    return 0;
}

ftime time(const fptr& file) 
{
    std::error_code ecode;
    const auto& result = time(file, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file time");
    return result;
}

ftime time(const fptr& file, std::error_code& error) noexcept 
{
    error.clear();
    
    ftime ft = { -1, -1, -1, -1 };

    if (file == nullptr) {
        error = MakeSysError(EINVAL);
        return ft;
    }

    // TODO
    error = make_error(kNotSupported);

    return ft;
}

ftime time(const path& name) 
{
    std::error_code ecode;
    const auto& result = time(name, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file time", name);
    return result;
}

ftime time(const path& name, std::error_code& error) noexcept 
{
    error.clear();

    if (name.empty()) {
        error = MakeSysError(EINVAL);
        return { -1, -1, -1, -1};
    }

    // TODO
    error = make_error(kNotSupported); // nothing
    }

    return ft;
}

void set_time(const fptr& file, const ftime& time) {
    std::error_code ecode;
    set_time(file, time, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to set the file time");
}

void set_time(const fptr& file, const ftime& time, std::error_code& error) noexcept 
{
    error.clear();
    if (file == nullptr) {
        error = MakeSysError(EINVAL);
        return;
    }

    // TODO
    error = make_error(kNotSupported);
}

void set_time(const path& name, const ftime& time) 
{
    std::error_code ecode;
    set_time(name, time, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to set the file time", name);
}

void set_time(const path& name, const ftime& time, std::error_code& error) noexcept 
{
    error.clear();
    if (name.empty()) {
        error = MakeSysError(EINVAL);
        return;
    }

    // TODO
    error = make_error(kNotSupported);
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