// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#include "utility/error.h"
#include "utility/string.h"
#include <format>

#if OS_WIN
#   include <Windows.h>
#   ifndef ERROR_NO_SUCH_DEVICE  // SDK v7.1A 中没有定义这个错误码
#      define ERROR_NO_SUCH_DEVICE 433L
#   endif
#else
#   include <errno.h>
#endif

namespace UTILITY_NAMESPACE {

std::string error_category::message(int ev) const {
    if (ev == kSucceed)
        return "Succeed";
    return sformat("Error: 0x%08x", ev);
}

#if OS_WIN
namespace win {
inline bool is_network_error(int ecode)
{
    switch (ecode)
    {
    case ERROR_REM_NOT_LIST:
    case ERROR_DUP_NAME:
    case ERROR_BAD_NETPATH:
    case ERROR_NETWORK_BUSY:
    case ERROR_DEV_NOT_EXIST:
    case ERROR_TOO_MANY_CMDS:
    case ERROR_ADAP_HDW_ERR:
    case ERROR_BAD_NET_RESP:
    case ERROR_UNEXP_NET_ERR:
    case ERROR_NETNAME_DELETED:
    case ERROR_NETWORK_ACCESS_DENIED:
    case ERROR_BAD_DEV_TYPE:
    case ERROR_BAD_NET_NAME:
    case ERROR_TOO_MANY_NAMES:
    case ERROR_TOO_MANY_SESS:
    case ERROR_SHARING_PAUSED:
    
    // 2021-6-21 添加
    case ERROR_NO_NETWORK:
    case ERROR_CONNECTION_REFUSED:
    case ERROR_CONNECTION_INVALID:
    case ERROR_NETWORK_UNREACHABLE:
    case ERROR_HOST_UNREACHABLE:
    case ERROR_PROTOCOL_UNREACHABLE:
    case ERROR_PORT_UNREACHABLE:
    case ERROR_REQUEST_ABORTED:
    case ERROR_CONNECTION_ABORTED:
    case ERROR_CONNECTION_COUNT_LIMIT:
        return true;
    }

    return false;
}

} // namespace win

std::error_code make_error_from_native(
    const int ecode,
    const std::filesystem::path& filename,
    const error defaultCode)
{
    switch (ecode)
    {
    case ERROR_DISK_FULL:       // 磁盘空间不足或不支持大文件
    {
        if (!filename.empty())  // 若文件名不为空, 则探测下是否是不支持大文件
        {

            #if 0
            auto fstype = util::path_filesystem(filename);
            if (fstype == util::FAT16 || fstype == util::FAT32)
            {
                namespace fs = std::filesystem;
                auto fname = filename;
                if (!fs::is_directory(fname))
                    fname = fname.parent_path().lexically_normal();
                fs::space_info space = fs::space(fname);
                if (!ecode && space.free > 0x200000) // 2MB
                    return make_error(kFilesystemNotSupportLargeFiles);
            }
            #endif
        }

        return make_error(kFilesystemNoSpace);
    }

    case ERROR_ACCESS_DENIED:
        return make_error(kPermissionError);

    case ERROR_PATH_NOT_FOUND:
    case ERROR_FILE_NOT_FOUND:
        return make_error(kFileNotFound);

    case ERROR_WRONG_DISK:
    case ERROR_FILE_INVALID:    // 文件所在的卷已被外部更改，因此打开的文件不再有效。
    case ERROR_NO_SUCH_DEVICE:  // 没有这样的设备, U 盘突然被拔出
        return make_error(kFilesystemUnavailable);

    case ERROR_INVALID_NAME:    // 无效文件名, 语法无效或太长, 这里排除bug那么只剩下路径过长
        return make_error(kFilesystemPathTooLong);

    default:
        if (win::is_network_error(ecode))
        {
            if (!filename.empty())
                return make_error(kFilesystemNetworkError);
            else
                return make_error(kNetworkError);
        }
    }

    return make_error(defaultCode);
}
#elif OS_POSIX
std::error_code make_error_from_native(
    const int ecode,
    const std::filesystem::path& filename,
    const error defaultCode)
{
    // Refer: https://man7.org/linux/man-pages/man3/errno.3.html
    // errno 中的值只有在调用的返回值表示错误时, 才有意义(即，大多数系统调用返回 -1; 大多数库函数返回 -1 或 NULL);
    // 一个成功的函数被允许修改 errno。任何系统调用或库函数都不会将 errno 的值设置为 0。
    // 
    switch(ecode)
    {
        case EACCES:
         return make_error(kPermissionError);

        case EINTR:
         return make_error(kInterruptedError);

        case EINVAL:
         return make_error(kInvalidParam);

    }

    // TODO
    // 实现 POSIX
    return make_error(defaultCode);
}
#endif

} // namespace UTILITY_NAMESPACE