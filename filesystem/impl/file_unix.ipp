#ifdef UTILITY_DISABLE_HEADONLY
#   include "../file_unix.hpp"
#endif

#include <ftw.h>
#include <fcntl.h>
#include <stdio.h>
#include <utime.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <iostream>
#include <boost/filesystem.hpp>
#include <platform/platform_util.hpp>

namespace util {

void ffile::close()
{
    if (vaild())
    {
        // 
        // https://linux.die.net/man/2/close

        if (::close(reinterpret_cast<int>(native_id())) == -1)
        {
            std::string message = format_error(errno);
            std::cerr << "Failed to close file descriptor: " << message << std::endl;
        }
    }

    _native_id = -1;
    _open_flags = 0;
}

bool file_exist(const fpath& name)
{
    ferror ferr;
    bool result = file_exist(name, ferr);

    if (ferr)
        throw ferr;

    return result;
}

bool file_exist(const fpath& name, ferror& ferr) noexcept
{
    ferr.clear();

    struct stat statbuf = {0};

    // 如果name指向一个个符号链接, 那么stat()将会继续使用该连接的内容解析路径名.
    //
    // 成功返回 0. 失败返回-1, 并设置errno.
    // https://linux.die.net/man/3/stat

    int ecode  = 0;
    int status = ::stat(name.c_str(), &statbuf);

    if (status == -1)
    {
        switch (ecode = errno)
        {
        case ENOENT:    // A component of path does not name an existing file or path is an empty string.
            break;

        default:
            ferr = ferror(ecode, "Can't get file status, stat() failed.");
            break;
        }

        return false;
    }

    return true;
}

void file_remove(const fpath& name)
{
    ferror ferr;
    file_remove(name, ferr);

    if (ferr)
        throw ferr;
}

void file_remove(const fpath& name, ferror& ferr) noexcept
{
    ferr.clear();

    // https://linux.die.net/man/2/unlink
    //

    if (::unlink(name.c_str()) == -1)
    {
        ferr = ferror(errno, "Can't delete file, unlink() failed.");
    }
}

void file_copy(const fpath& from, const fpath& to)
{
    ferror ferr;
    file_copy(from, to, ferr);

    if (ferr)
        throw ferr;
}

void file_copy(const fpath& from, const fpath& to, ferror& ferr) noexcept
{
    ferr.clear();

    boost::system::error_code ecode;
    boost::filesystem::copy_file(
        boost::filesystem::path(from), 
        boost::filesystem::path(to),
        boost::filesystem::copy_option::overwrite_if_exists, ecode);

    if (ecode)
    {
        ferr = ferror(ecode.value(), "Can't copy file, copy_file() failed.");
    }
}

void file_move(const fpath& from, const fpath& to)
{
    ferror ferr;
    file_move(from, to, ferr);

    if (ferr)
        throw ferr;
}

void file_move(const fpath& from, const fpath& to, ferror& ferr) noexcept
{
    ferr.clear();

    if (!file_exist(from, ferr))
        return;

    if (file_exist(to, ferr))
    {
        ferr = ferror(-1, "Destination file exists");
        return;
    }

    // https://linux.die.net/man/2/rename
    //

    if (::rename(from.c_str(), to.c_str()) == -1)
    {
        file_copy(from, to, ferr);
        if (ferr)
            return;

        file_remove(from, ferr);
        if (ferr)
        {
            int code = ferr.code();
            file_remove(to, ferr);

            ferr = ferror(code, "Can't move file, remove source file failed.");
        }
    }
}

fsize file_size(const fpath& name)
{
    ferror ferr;
    fsize  result = file_size(name, ferr);

    if (ferr)
        throw ferr;

    return result;
}

fsize file_size(const fpath& name, ferror& ferr) noexcept
{
    ferr.clear();

    struct stat statbuf = { 0 };

    int ecode = 0;
    int status = ::stat(name.c_str(), &statbuf);

    if (status == -1)
    {
        ferr = ferror(ecode, "Can't get file size, stat() failed.");
        return 0;
    }

    return statbuf.st_size;
}

ftime file_time(const fpath& name)
{
    ferror ferr;
    ftime  result = file_time(name, ferr);

    if (ferr)
        throw ferr;

    return result;
}

ftime file_time(const fpath& name, ferror& ferr) noexcept
{
    ferr.clear();

    struct stat statbuf = { 0 };

    int ecode = 0;
    int status = ::stat(name.c_str(), &statbuf);

    if (status == -1)
    {
        ferr = ferror(ecode, "Can't get file time, stat() failed.");

        ftime ft = { -1, -1, -1, -1};
        return ft;
    }

    ftime ft = { 
#ifdef HAVE_ST_BIRTHTIME
        statbuf.st_birthtime,
#else
        -1,
#endif
        statbuf.st_atime,
        statbuf.st_mtime,
        statbuf.st_ctime,
    };

    return ft;
}

void file_set_time(const fpath& name, const ftime& time)
{
    ferror ferr;
    file_set_time(name, time, ferr);

    if (ferr)
        throw ferr;
}

void file_set_time(const fpath& name, const ftime& time, ferror& ferr) noexcept
{
    ferr.clear();

    struct stat    statbuf = { 0 };
    struct utimbuf timebuf = { 0 };

    if(time.access_time == -1 || time.modify_time == -1)
    {
        int status = ::stat(name.c_str(), &statbuf);

        if (status == -1)
        {
            ferr = ferror(errno, "Can't get file status, stat() failed.");
            return;
        }
    }

    timebuf.actime  = time.access_time;
    timebuf.modtime = time.modify_time;

    if (time.access_time == -1)
        timebuf.actime = statbuf.st_atime;

    if (time.modify_time == -1)
        timebuf.modtime = statbuf.st_mtime;

    if (time.change_time != -1)
        std::cerr << "The change_time field is discarded on the target platform" << std::endl;

    if (time.create_time != -1)
        std::cerr << "The create_time field is discarded on the target platform" << std::endl;

    // 
    // https://linux.die.net/man/2/utime
    //
    if (::utime(name.c_str(), &timebuf) == -1)
    {
        ferr = ferror(errno, "Can't set file times, utime() failed.");
    }
}

ffile file_open(const fpath& name, int flags)
{
    ferror ferr;
    ffile result = file_open(name, flags, ferr);

    if (ferr)
        throw ferr;

    return result;
}

ffile file_open(const fpath& name, int flags, ferror& ferr) noexcept
{
    ferr.clear();

    // 
    // 在操作完成时, 该函数将打开文件并返回表示文件描述符的非负整数(一个进程文件描述符表中未使用的最低编号).
    // 其他情况返回-1, 并设置errno, 此时不会创建或修改文件;
    //
    // https://linux.die.net/man/2/open

    int fid = ::open(
        name.c_str(), 
        flags, 
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    if(fid == -1) // error
    {
        ferr = ferror(errno, "Can't open file");
        return ffile();
    }

    return ffile(fid, flags);
}

void file_close(ffile& file) noexcept
{
    file.close();
}

void file_read(const ffile& file, void* data_out, int size)
{
    ferror ferr;
    file_read(file, data_out, size, ferr);

    if (ferr)
        throw ferr;
}

void file_read(const ffile& file, void* data_out, int size, ferror& ferr) noexcept
{
    ferr.clear();

    if (!file.vaild())
    {
        ferr = ferror(-1, "Invalid file handle");
        return;
    }

    // 系统调用被信号中断后继续尝试之
    // https://linux.die.net/man/2/read
    // 

    int bytesRead = 0;
    do
    {
        bytesRead = ::read(file, data_out, static_cast<size_t>(size));

        if(bytesRead >= 0)
            break;
    } 
    while (errno == EINTR);
    
    if (bytesRead == -1)
    {
        ferr = ferror(errno, "Can't read file data, read() failed.");
        return;
    }

    if (static_cast<int>(bytesRead) != size)
    {
        ferr = ferror(-1, "The file was read successfully, but it was too short");
    }
}

void file_write(ffile& file, const void *data, int size)
{
    ferror ferr;
    file_write(file, data, size, ferr);

    if (ferr)
        throw ferr;
}

void file_write(ffile& file, const void *data, int size, ferror& ferr) noexcept
{
    ferr.clear();

    if (!file.vaild())
    {
        ferr = ferror(-1, "Invalid file handle");
        return;
    }

    // 
    // https://linux.die.net/man/2/write
    //

    int bytesWritten = 0;
    do
    {
        bytesWritten = ::write(file, data, static_cast<size_t>(size));

        if (bytesWritten >= 0)
            break;
    } while (errno == EINTR);

    if (bytesWritten == -1 || bytesWritten != size)
    {
        ferr = ferror(errno, "File write failed, write() failed.");
    }
}

void file_seek(ffile& file, fsize offset, int whence/* = SEEK_SET*/)
{
    ferror ferr;
    file_seek(file, offset, whence, ferr);

    if (ferr)
        throw ferr;
}

void file_seek(ffile& file, fsize offset, int whence, ferror& ferr) noexcept
{
    ferr.clear();

    if (!file.vaild())
    {
        ferr = ferror(-1, "Invalid file handle");
        return;
    }

    // 
    // https://linux.die.net/man/2/lseek

    fsize pos = static_cast<long long>(lseek(file, offset, whence));

    if (pos == -1)
    {
        ferr = ferror(errno, "Cannot move file pointer, lseek() failed.");
    }
}

fsize file_tell(const ffile& file)
{
    ferror ferr;
    int64_t result = file_tell(file, ferr);

    if (ferr)
        throw ferr;

    return result;
}

fsize file_tell(const ffile& file, ferror& ferr) noexcept
{
    ferr.clear();
    if (!file.vaild())
    {
        ferr = ferror(-1, "Invalid file handle");
        return 0;
    }

    fsize pos = static_cast<long long>(lseek(file, 0, SEEK_CUR));

    if (pos == -1)
    {
        ferr = ferror(errno, "Cannot get file pointer, lseek() failed.");
    }

    return pos;
}

fsize file_size(const ffile& file)
{
    ferror ferr;
    fsize result = file_size(file, ferr);

    if (ferr)
        throw ferr;

    return result;
}

fsize file_size(const ffile& file, ferror& ferr) noexcept
{
    ferr.clear();
    if (!file.vaild())
    {
        ferr = ferror(-1, "Invalid file handle");
        return 0;
    }

    //
    // https://linux.die.net/man/2/lseek

    fsize curr = 0, size = 0;
    do
    {
        curr = static_cast<long long>(lseek(file, 0, SEEK_CUR));
        if (curr == -1)
            break;

        size = static_cast<long long>(lseek(file, 0, SEEK_END));
        if (size == -1)
            break;

        if (lseek(file, curr, SEEK_SET) == -1)
        {
            ferr = ferror(errno, "Cannot restore file pointer, lseek() failed.");
            return size;
        }
    } 
    while (0);

    if (curr == -1 || size == -1)
    {
        ferr = ferror(errno, "Cannot get file size, lseek() failed.");
    }

    return size;
}

ftime file_time(const ffile& file)
{
    ferror ferr;
    ftime result = file_time(file, ferr);

    if (ferr)
        throw ferr;

    return result;
}

ftime file_time(const ffile& file, ferror& ferr) noexcept
{
    ferr.clear();
    ftime ft = { -1, -1, -1, -1 };

    if (!file.vaild())
    {
        ferr = ferror(-1, "Invalid file handle");
        return ft;
    }

    // 
    // https://linux.die.net/man/2/fstat

    struct stat statbuf = { 0 };

    if (::fstat(file, &statbuf) == -1)
    {
        ferr = ferror(errno, "Can't get file time, fstat() failed.");
        return ft;
    }

    ft.access_time = statbuf.st_atime;
    ft.modify_time = statbuf.st_mtime;
    ft.change_time = statbuf.st_ctime;

#ifdef HAVE_ST_BIRTHTIME
    ft.create_time = statbuf.st_birthtime;
#endif

    return ft;
}

void file_set_time(const ffile& file, const ftime& time)
{
    ferror ferr;
    file_set_time(file, time, ferr);

    if (ferr)
        throw ferr;
}

void file_set_time(const ffile& file, const ftime& time, ferror& ferr) noexcept
{
    ferr.clear();
    if (!file.vaild())
    {
        ferr = ferror(-1, "Invalid file handle");
        return;
    }

    struct timespec times[2] = { 0 };

    if (time.access_time == -1)
        times[0].tv_sec = time.access_time;
    else
        times[0].tv_nsec = UTIME_OMIT;

    if (time.modify_time == -1)
        times[1].tv_sec = time.modify_time;
    else
        times[1].tv_nsec = UTIME_OMIT;

    if (time.change_time != -1)
        std::cerr << "The change_time field is discarded on the target platform" << std::endl;

    if (time.create_time != -1)
        std::cerr << "The create_time field is discarded on the target platform" << std::endl;

    // 
    // https://linux.die.net/man/2/utimensat


    if (::futimens(file, times) == -1)
    {
        ferr = ferror(errno, "Can't set file time, futimens() failed.");
    }
}

bool file_is_writable(const fpath& name)
{
    ferror ferr;
    bool result = file_is_writable(name, ferr);

    if (ferr)
        throw ferr;

    return result;
}

bool file_is_writable(const fpath& name, ferror& ferr) noexcept
{
    ferr.clear();
    if (!file_exist(name, ferr))
        return path_is_writable(name, ferr);

    bool result = !!file_open(name, O_WRONLY, ferr);

    if (ferr)
    {
        if (ferr.code() == EACCES)
            ferr.clear();
    }
        
    return result;
}

void directories_remove(const fpath& path)
{
    ferror ferr;
    directories_remove(path, ferr);

    if (ferr)
        throw ferr;
}

#if 0
namespace detail {

    inline bool remove_file(
        const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf) noexcept
    {
        // 
        // https://linux.die.net/man/3/remove
        // 
        return ::remove(path);
    }

} // deatil
#endif

void directories_remove(const fpath& path, ferror& ferr) noexcept
{
    ferr.clear();

#if 0
    // 
    // https://linux.die.net/man/3/nftw
    // 
    // 如果fn()返回非零值，则树遍历结束，并且fn()返回的值作为ftw()或nftw()的结果返回。
    //
    // FTW_DEPTH 如果设置, 则执行一个后序遍历，即在处理目录及其子目录的内容之后，对目录本身调用fn()。
    //           (默认情况下，每个目录在其内容之前处理)
    // FTW_PHYS  如果设置, 则不解析符号链接. 默认会解析符号链接, 但不会调用两次fn().

    if (::nftw(path.c_str(), detail::remove_file, 64, FTW_DEPTH | FTW_PHYS) != 0)
    {
        ferr = ferror(errno, "Can't remove the directories");
    }
#endif


    // 
    // 暂时使用boost的实现, 后面如有需要在行实现.
    // remove_all() 返回出的文件数量
    boost::system::error_code ecode;
    boost::filesystem::remove_all(path, ecode);

    if (ecode)
    {
        ferr = ferror(ecode.value(), "Can't remove the directories, remove_all() falied");
    }
}

void directories_create(const fpath& path)
{
    ferror ferr;
    directories_create(path, ferr);

    if (ferr)
        throw ferr;
}

void directories_create(const fpath& path, ferror& ferr) noexcept
{
    ferr.clear();

    if (file_exist(path, ferr) || path_is_root(path))
        return;

    fpath pdir = path;

    if (!file_exist(pdir = path_find_parent(pdir), ferr))
    {
        directories_create(pdir, ferr);
        if (ferr) 
            return;
    }

    //
    // https://linux.die.net/man/2/mkdir

    if (::mkdir(path.c_str(), 0775) == -1)
    {
        switch (errno)
        {
        case EEXIST: // 已经存在(不一定是一个目录)
            break;

        default:
            ferr = ferror(errno, "Can't create directories, mkdir() failed");
            break;
        }
    }
}

} // util