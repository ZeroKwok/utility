// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.
//
// v0.1 2018-03
// v0.2 2019-07
// v0.3 2020-12
// v0.4 2024-06

#ifndef filesystem_win_h__
#define filesystem_win_h__

#include "utility/error.h"
#include "utility/filesystem.h"
#include "utility/filesystem_path.h"

namespace UTILITY_FS_NAMESPACE {
namespace win {

/*!
 *  \brief Locks the specified file for exclusive access by the calling process.
 * 
 *  \param file   The file handle by the file_open returned.
 *  \param offset The 64 bit of the starting byte offset in the file where the lock should begin.
 *  \param length The 64 bit of the length of the byte range to be locked.
 *  \return If the function succeeds, the return true.
 *
 *  \note Locking a region of a file gives the threads of the locking process exclusive access to
 *        the specified region using this file handle. If the file handle is inherited by a process
 *        created by the locking process, the child process is not granted access to the locked region.
 *        If the locking process opens the file a second time, it cannot access the specified region
 *        through this second handle until it unlocks the region.
 */
UTILITY_API bool file_lock(file& file, size offset, size length);
UTILITY_API bool file_lock(file& file, size offset, size length, std::error_code& error)  noexcept;

/*!
 *  \brief Unlocks a region in an open file. Unlocking a region enables other processes to access the region.
 *  \param file The file handle by the file_open returned.
 *  \param offset The 64 bit of the starting byte offset in the file where the locked region begins.
 *  \param length The 64 bit of the length of the byte range to be locked.
 *  \return If the function succeeds, the return true.
 *
 *  \note If a process terminates with a portion of a file locked or closes a file that has outstanding
 *        locks, the locks are unlocked by the operating system. However, the time it takes for the
 *        operating system to unlock these locks depends upon available system resources. Therefore,
 *        it is recommended that your process explicitly unlock all files it has locked when it terminates.
 *        If this is not done, access to these files may be denied if the operating system has not yet
 *        unlocked them.
 */
UTILITY_API bool file_unlock(file& file, size offset, size length);
UTILITY_API bool file_unlock(file& file, size offset, size length, std::error_code& error) noexcept;

/*!
 *  \brief 判断文件是否被其他进程占用
 * 
 *  \note  1. 如果name指向一个符号链接, 则将通过其内容解析目标文件.
 *         2. 原理是判断文件是否被其他进程以独占写打开, 故判断结果可能并不精确.
 */
UTILITY_API bool file_is_occupied(const path& name);
UTILITY_API bool file_is_occupied(const path& name, std::error_code& error) noexcept;

/*!
 *  \brief 在windows的shell中打开文件, 调用该文件关联的应用程序.
 * 
 *  \note  1. 如果name指向一个符号链接, 则将通过其内容解析目标文件.
 *         2. 相当于在explorer中双击打开该文件类似, 若该文件没有与之关联的应用程序将会出错.  
 */
UTILITY_API bool file_open_with_shell(const path& name);
UTILITY_API bool file_open_with_shell(const path& name, std::error_code& error) noexcept;

//! 与windows平台相关联的文件版本结构体
//! 
struct fversion
{
    int       file_flag_mask;  //!< 如: 0x3f 
    int       file_flags;      //!< 如: 0x00
    int       file_os;         //!< 如: VOS_NT_WINDOWS32
    int       file_type;       //!< 如: VFT_APP
    int       file_sub_type;   //!< 如: VFT2_UNKNOWN

    union version_t {
        struct {
            short major;       //!< 主版本号 
            short minor;       //!< 次版本号
            short patch;       //!< 修订版本号
            short build;       //!< 构建版本号
        };
        int64_t version;       //!< 64位版本号
    };
    version_t file_version;    //!< 文件版本
    version_t product_version; //!< 产品版本
};

UTILITY_API fversion file_version(const path& name);
UTILITY_API fversion file_version(const path& name, std::error_code& error) noexcept;

/*!
 *  \brief 将指定目录向标准用户授予写权限
 */
UTILITY_API void directory_authorization(const path& path);
UTILITY_API void directory_authorization(const path& path, std::error_code& error) noexcept;

/*!
 *  \brief 创建windows link文件(快捷方式)
 *  \param hot_key 如: (HOTKEYF_CONTROL | HOTKEYF_ALT) << 8 | 0x42 (虚拟键码 B)
 *         参考: https://docs.microsoft.com/zh-cn/windows/win32/api/shobjidl_core/nf-shobjidl_core-ishelllinka-gethotkey
 *  \param show_cmd
 *         参考: https://docs.microsoft.com/zh-cn/windows/win32/api/winuser/nf-winuser-showwindow
 */
UTILITY_API void link_create(
    const path& file_path,
    const path& link_path,
    const path& icon_path = L"",
    const path& run_cmd   = L"",
    const short hot_key   = 0,
    const int   show_flag = 0);
UTILITY_API void link_create(
    const path& file_path,
    const path& link_path,
    const path& icon_path,
    const path& run_cmd,
    const short hot_key,
    const int   show_flag,
    std::error_code  & error) noexcept;

//! 文件系统类型
enum fstype
{
    kFSTypeUnknown   = 0,
    kFSTypeFAT       = 1,
    kFSTypeFAT16     = 2,
    kFSTypeFAT32     = 3,
    kFSTypeexFAT     = 4,
    kFSTypeNTFS      = 5,
    kFSTypeCDFS      = 6,
};

/*!
 *  \brief 获取路径所在的文件系统类型.
 *  
 *  \note  暂不支持Unix-like.
 */
UTILITY_API fstype filesystem_type(const path& path);
UTILITY_API fstype filesystem_type(const path& path, std::error_code& error) noexcept;

} // win
} // UTILITY_NAMESPACE

#endif // filesystem_win_h__
