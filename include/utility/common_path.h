// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.
//
// v0.1 2018-03
// v0.2 2019-07
// v0.3 2021-01
// v0.4 2024-06

#ifndef common_path_h__
#define common_path_h__

#include "error.h"

// 通用路径处理函数, 适用于
//
// 1. 简单/特殊 文件系统路径
// 2. 常规 URL

namespace UTILITY_NAMESPACE {

/*!
 *  \brief 判断是否为根目录 (root-directory)
 *
 *  \note  URL 路径不支持判断root.
 *         1. C:, C:\, C:/, \\?\C:     -> true
 *         2. E:\folder                -> false
 *         3. \\servername\share       -> true
 *         4. \\servername\share\      -> true
 *         5. \\servername\share\file  -> false
 *         6. \\servername\            -> false UNC路径不完整
 *         7. \\?\UNC\servername\share -> true
 *         8. /                        -> true
 *         9. /etc                     -> false
 */
UTILITY_API bool path_is_root(const path& path) noexcept;

/*!
 *  \brief 判断是否为UNC路径
 *  
 *  \note  仅在路径符合UNC规范时才会返回true.
 *         1. \\servername\share          -> true
 *         2. \\servername\               -> false 不完整路径
 *         3. \\servername\share\folder   -> true
 *         4. \\?\UNC\servername\share    -> true
 */
UTILITY_API bool path_is_unc(const path& path) noexcept;

/*!
 *  \brief 判断是否为URL路径
           检测是否包含: "://".
 */
UTILITY_API bool path_is_url(const path& path) noexcept;

/*!
 *  \brief 判断路径是否为Windows风格
 *      1. 检测到驱动器号 [a-zA-Z]: 驱动器号为一个字符，且前面只能存在'/'或'\\'字符.
 *      2. 否则检测到分隔符 \
 * 
 *  \note  仅判断路径是否为Windows风格, 不判断是否符合Windows标准.
 */
UTILITY_API bool path_is_win_style(const path& path) noexcept;

/*!
 *  \brief 判断路径是否为Windows的长路径
 *         如： 1. \\?\D:\very long path
 *             2. \\?\UNC\server\share
 * 
 *  \note  检测到前缀: \\?\
 */
UTILITY_API bool path_is_win_long_path(const path& path) noexcept;

/*!
 *  \brief 判断路径是否为Unix风格
 *         1. 检测到根目录 /
 *         2. 检测到分隔符 / 且为 非Windows 风格
 */
UTILITY_API bool path_is_unix_style(const path& path) noexcept;

/*!
 *  \brief 判断路径是否为UNC风格
 *         检测到  \\*\* 或 \\?\UNC\*\*
 * 
 *  \note  仅判断是否为UNC风格, 而不是判断路径是否符合UNC规范.
 *         Windows 长路径将返回 false, 如: \\?\D:\
 *  \see   path_is_unc().
 */
UTILITY_API bool path_is_unc_style(const path& path) noexcept;

/*!
 *  \brief 判断是否为远程路径 (不能判断断开连接的网络驱动器)
 *  
 *  \note  目前仅支持Windows平台, 其他平台总返回false.
 */
UTILITY_API bool path_is_remote(const path& path) noexcept;

/*!
 *  \brief 路径拼接
 *         根据不同路径风格适当的处理路劲分隔符
 * 
 *  \note  首先判断path的路径风格, 再根据不同风格执行不同的行为, 目前支持Window风格, Unix-Like风格, UNC风格, URL风格.
 *         由于Unix-Like 中 "\"可以作为文件名, 故在此类平台中不支持Windows, UNC风格的路径.
 *         具体如下表所示:
 *         ______________________________________________________________________________________________________
 *         |    path             |   stem                |   result                       | Windows | Unix-Like |
 *         | C:\                 |  \                    | C:\                            | Yes     | No        |
 *         | C:\                 |  Program Files        | C:\Program Files               | Yes     | No        |
 *         | C:\Program Files    |  Folder               | C:\Program Files\Folder        | Yes     | No        |
 *         | C:\Program Files    |  Folder\\File         | C:\Program Files\Folder\\File  | Yes     | No        |
 *         | C:\Program Files    |  \\Folder             | C:\Program Files\Folder        | Yes     | No        |
 *         | Empty               |  \Folder              | \Folder                        | Yes     | No        |
 *         | \\server\share      |  folder               | \\server\share\folder          | Yes     | No        |
 *         | /etc                |  passwd               | /etc/passwd                    | Yes     | Yes       |
 *         | https://example.com |  index.html           | https://example.com/index.html | Yes     | Yes       |
 *         ------------------------------------------------------------------------------------------------------
 *         另外需要注意的是, 该函数不会处理路径中重复的分隔符, 不会处理不符合规范的路径.
 */
UTILITY_API std::string  path_append(const std::string& path, const std::string& stem_1) noexcept;
UTILITY_API std::wstring path_append(const std::wstring& path, const std::wstring& stem_1) noexcept;
UTILITY_API std::string  path_append(const std::string& path, const std::string& stem_1, const std::string& stem_2) noexcept;
UTILITY_API std::wstring path_append(const std::wstring& path, const std::wstring& stem_1, const std::wstring& stem_2) noexcept;

/*!
 *  \brief 返回路径的根目录
 * 
 *  \note  根目录暂不支持URL路径, 具体规则如下:
 *         1. 本地路径: 驱动器    C:\Program Files\windows    -> C:\
 *                                \\?\C:\Program Files        -> \\?\C:\
 *                      根目录    /home/git                   -> /
 * 
 *         2. 网络路径: UNC根目录 \\192.168.0.13\share\folder -> \\192.168.0.13\share
 *                      UNC长路径 \\?\UNC\server\share\folder -> \\?\UNC\server\share
 * 
 *         由于Unix-Like 中 "\"可以作为文件名, 故在此类平台中不支持Windows, UNC风格的路径.
 */
UTILITY_API std::string  path_find_root(const std::string& path) noexcept;
UTILITY_API std::wstring path_find_root(const std::wstring& path) noexcept;

/*!
 *  \brief 返回路径目录
 * 
 *  \note  若path指向的路径是根路径, 那么返回自己, 具体规则如下:
 *         1. "C:\Program Files\windows"    -> "C:\Program Files"
 *         2. "\\192.168.0.13\share\folder" -> "\\192.168.0.13\share"
 *         3. "\\?\UNC\server\share"        -> "\\?\UNC\server\share"
 *         4. "\\?\UNC\server\share\folder" -> "\\?\UNC\server\share"
 *         5. "/tmp/archive.tar.gz"         -> "/tmp"
 *         6. "/"                           -> "/"
 * 
 *         由于Unix-Like 中 "\"可以作为文件名, 故在此类平台中不支持Windows, UNC风格的路径.
 */
UTILITY_API std::string  path_find_parent(const std::string& path) noexcept;
UTILITY_API std::wstring path_find_parent(const std::wstring& path) noexcept;

/*!
 *  \brief 返回路径的文件名
 *         "/tmp/archive.tar.gz" -> "archive.tar.gz"
 * 
 *  \note  由于Unix-Like 中 "\"可以作为文件名, 故在此类平台中不支持Windows, UNC风格的路径.
 */
UTILITY_API std::string  path_find_filename(const std::string& path) noexcept;
UTILITY_API std::wstring path_find_filename(const std::wstring& path) noexcept;

/*!
 *  \brief 返回路径的文件名(不含扩展名)
 *         "/tmp/archive.tar.gz" -> "archive"
 * 
 *  \note  由于Unix-Like 中 "\"可以作为文件名, 故在此类平台中不支持Windows, UNC风格的路径.
 */
UTILITY_API std::string  path_find_basename(const std::string& path) noexcept;
UTILITY_API std::wstring path_find_basename(const std::wstring& path) noexcept;

//! 查找扩展名标志位
enum find_extension_flag
{
    find_default        = 0x00,  //!< "archive.tar.gz" -> "gz"
    find_complete       = 0x01,  //!< "archive.tar.gz" -> "tar.gz"   完整扩展名,    默认最后一级      
    find_upper_case     = 0x02,  //!< "archive.txt"    -> "TXT"      扩展名为大写,  默认小写          
    find_with_dot       = 0x04,  //!< "archive.tar.gz" -> ".gz"      扩展名包含'.', 默认不包含                   
};

/*!
 *  \brief 查找扩展名
 * 
 *  \note  查找规则参见: find_extension_flag
 */
UTILITY_API std::string  path_find_extension(const std::string& path, int flags = find_default) noexcept;
UTILITY_API std::wstring path_find_extension(const std::wstring& path, int flags = find_default) noexcept;

} // UTILITY_NAMESPACE

#endif // common_path_h__