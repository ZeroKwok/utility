// This file is part of the utility project.
// Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#ifndef error_win_h__
#define error_win_h__

#include <Windows.h>
#ifndef ERROR_NO_SUCH_DEVICE // SDK v7.1A 中没有定义这个错误码
#   define ERROR_NO_SUCH_DEVICE 433L
#endif

#include "utility/error.h"

namespace UTILITY_NAMESPACE
{
    namespace win
    {
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

            // 2021-6-21
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

        enum fstype
        {
            None  = 0,
            FAT   = 1,
            FAT16 = 2,
            FAT32 = 3,
            exFAT = 4,
            NTFS  = 5,
            CDFS  = 6,
            Other = 7
        };

        inline fstype filesystem_type(const std::filesystem::path& path, std::error_code& error)
        {
            auto root = path.lexically_normal().root_path();
            std::wstring type(10, 0);
            if (!GetVolumeInformationW(
                root.wstring().c_str(), nullptr, 0, nullptr, nullptr, nullptr, 
                &type[0], type.size()))
            {
                error = make_error_from_native(::GetLastError());
                return None;
            }

            // FAT（FAT12\FAT16和FAT32）、NTFS、CDFS、exFAT、
            // RAW、Ext、Btrfs、ZFS、HFS、 HFS+、ReiserFS、JFS、VMFS、XFS、UFS、VXFS、ReFS、WBFS、PFS
            // 
            if (type.starts_with(L"NTFS"))
                return NTFS;
            else if (type.starts_with(L"FAT32"))
                return FAT32;
            else if (type.starts_with(L"FAT16"))
                return FAT16;
            else if (type.starts_with(L"FAT"))
                return FAT;
            else if (type.starts_with(L"CDFS"))
                return CDFS;
            else if (type.starts_with(L"exFAT"))
                return exFAT;
            else
                return Other;
        }

    } // namespace win

    std::error_code make_error_from_native(
        const int ecode,
        const std::filesystem::path &filename,
        const error defaultCode)
    {
        switch (ecode)
        {
        case ERROR_SUCCESS:
                return make_error(kSucceed);

        case ERROR_DISK_FULL:       // 磁盘空间不足或不支持大文件
        {
            if (!filename.empty())  // 若文件名不为空, 则探测下是否是不支持大文件
            {
                std::error_code ec;
                auto type = win::filesystem_type(filename, ec);
                if (!ec && (type == win::FAT16 || type == win::FAT32))
                {
                    namespace fs = std::filesystem;
                    auto path = filename;
                    if (!fs::is_directory(path))
                        path = path.lexically_normal().parent_path();
                    auto space = fs::space(path, ec);
                    if (!ec && space.free > 0x200000) // 2MB
                        return make_error(kFilesystemNotSupportLargeFiles);
                }
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

} // namespace UTILITY_NAMESPACE

#endif // error_win_h__