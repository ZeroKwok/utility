#ifndef path_util_h__
#define path_util_h__

/*
*   path_util.h
*
*   v0.1 2018-3 by GuoJH
*   v0.2 2019-7 by GuoJH
*   v0.3 2021-1 by GuoJH
*/

#include <filesystem/file_util.h>
#include <filesystem/filesystem_cfg.h>

#if OS_WIN
#   include <shlobj.h>
#   ifdef MessageBox
#       undef MessageBox
#   endif
#endif

namespace util {

/*!
 *  \brief 返回模块目录的路径工厂函数
 * 
 *  \param instance 模块实例id, 0代表当前模块.
 *  \param stem 可以是文件名, 也可以是目录名, 附加在结果路径之后.
 *  
 *  \note  在windows中instance代表的是模块句柄(HMODULE),
 *         而linux  中instance代表的是进程id(pid_t)
 *         在不支持 /proc 文件系统的平台, path_from_module(), path_from_module_dir() 返回程序的当前目录.
 */
UTILITY_FUNCT_DECL fpath path_from_module(intptr_t instance = 0);
UTILITY_FUNCT_DECL fpath path_from_module(intptr_t instance, ferror& ferr) noexcept;
UTILITY_FUNCT_DECL fpath path_from_module_dir(intptr_t instance = 0);
UTILITY_FUNCT_DECL fpath path_from_module_dir(intptr_t instance, ferror& ferr) noexcept;

/*!
 *  \brief 返回模块目录的便捷API
 *         相当于: return path_append(path_from_module_dir(0, ferr), stem);
 */
UTILITY_FUNCT_DECL fpath path_from_module_dir(const fpath& stem);
UTILITY_FUNCT_DECL fpath path_from_module_dir(const fpath& stem, ferror& ferr) noexcept;

/*!
 *  \brief  返回系统的临时目录
 *
 *  \return 在Windows中相当于环境变量 "%Temp%", 而Unix相当于 "$TMPDIR".
 *  \note   在Unix-Like中若不存在"TMPDIR"环境变量，则尝试匹配P_tmpdir宏的值，若编译平台没有提供该定义，
 *          则将返回“/tmp”.
 */
UTILITY_FUNCT_DECL fpath path_from_temp();
UTILITY_FUNCT_DECL fpath path_from_temp(ferror& ferr) noexcept;
UTILITY_FUNCT_DECL fpath path_from_temp(const fpath& stem);
UTILITY_FUNCT_DECL fpath path_from_temp(const fpath& stem, ferror& ferr) noexcept;

/*!
 *  \brief  返回进程用户的家目录
 *
 *  \return 在Windows中相当于环境变量 "%USERPROFILE%", 而Unix相当于 "~".
 */
UTILITY_FUNCT_DECL fpath path_from_home();
UTILITY_FUNCT_DECL fpath path_from_home(ferror& ferr) noexcept;
UTILITY_FUNCT_DECL fpath path_from_home(const fpath& stem);
UTILITY_FUNCT_DECL fpath path_from_home(const fpath& stem, ferror& ferr) noexcept;

/*!
 *  \brief 判断是否为Root目录
 *
 *  \note  URL路径不支持判断root.
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
UTILITY_FUNCT_DECL bool path_is_root(const fpath& path) noexcept;

/*!
 *  \brief 判断是否为UNC路径
 *  
 *  \note  仅在路径符合UNC规范时才会返回true.
 *         1. \\servername\share          -> true
 *         2. \\servername\               -> false 不完整路径
 *         3. \\servername\share\folder   -> true
 *         4. \\?\UNC\servername\share    -> true
 */
UTILITY_FUNCT_DECL bool path_is_unc(const fpath& path) noexcept;

/*!
 *  \brief 判断是否为URL路径
           检测是否包含: "://".
 */
UTILITY_FUNCT_DECL bool path_is_url(const fpath& path) noexcept;

/*!
 *  \brief 判断路径是否为Windows风格
 *      1. 检测到驱动器号 [a-zA-Z]: 驱动器号为一个字符，且前面只能存在'/'或'\\'字符.
 *      2. 否则检测到分隔符 \
 * 
 *  \note  仅判断路径是否为Windows风格, 不判断是否符合Windows标准.
 */
UTILITY_FUNCT_DECL bool path_is_win_style(const fpath& path) noexcept;

/*!
 *  \brief 判断路径是否为Windows的长路径
 *         如： 1. \\?\D:\very long path
 *             2. \\?\UNC\server\share
 * 
 *  \note  检测到前缀: \\?\
 */
UTILITY_FUNCT_DECL bool path_is_win_long_path(const fpath& path) noexcept;

/*!
 *  \brief 判断路径是否为Unix风格
 *         1. 检测到根目录 /
 *         2. 检测到分隔符 / 且为 非Windows 风格
 */
UTILITY_FUNCT_DECL bool path_is_unix_style(const fpath& path) noexcept;

/*!
 *  \brief 判断路径是否为UNC风格
 *         检测到  \\*\* 或 \\?\UNC\*\*
 * 
 *  \note  仅判断是否为UNC风格, 而不是判断路径是否符合UNC规范.
 *         Windows 长路径将返回 false, 如: \\?\D:\
 *  \see   path_is_unc().
 */
UTILITY_FUNCT_DECL bool path_is_unc_style(const fpath& path) noexcept;

/*!
 *  \brief 判断是否为远程路径 (不能判断断开连接的网络驱动器)
 *  
 *  \note  目前仅支持Windows平台, 其他平台总返回false.
 */
UTILITY_FUNCT_DECL bool path_is_remote(const fpath& path) noexcept;

/*!
 *  \brief 判断路径是否可写
 *         若指定路径不存在, 则向上查找第一个存在的父目录并判断是否可写.
 */
UTILITY_FUNCT_DECL bool path_is_writable(const fpath& path);
UTILITY_FUNCT_DECL bool path_is_writable(const fpath& path, ferror& ferr) noexcept;

//! 文件系统类型
enum fstype
{
    none   = 0,
    FAT16  = 1,
    FAT32  = 2,
    exFAT  = 3,
    NTFS   = 4,
};

/*!
 *  \brief 获取路径所在的文件系统类型.
 *  
 *  \note  暂不支持Unix-like.
 */
UTILITY_FUNCT_DECL fstype path_filesystem(const fpath& path);
UTILITY_FUNCT_DECL fstype path_filesystem(const fpath& path, ferror& ferr) noexcept;

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
UTILITY_FUNCT_DECL std::string  path_append(const std::string& path, const std::string& stem_1) noexcept;
UTILITY_FUNCT_DECL std::wstring path_append(const std::wstring& path, const std::wstring& stem_1) noexcept;
UTILITY_FUNCT_DECL std::string  path_append(const std::string& path, const std::string& stem_1, const std::string& stem_2) noexcept;
UTILITY_FUNCT_DECL std::wstring path_append(const std::wstring& path, const std::wstring& stem_1, const std::wstring& stem_2) noexcept;

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
UTILITY_FUNCT_DECL std::string  path_find_root(const std::string& path) noexcept;
UTILITY_FUNCT_DECL std::wstring path_find_root(const std::wstring& path) noexcept;

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
UTILITY_FUNCT_DECL std::string  path_find_parent(const std::string& path) noexcept;
UTILITY_FUNCT_DECL std::wstring path_find_parent(const std::wstring& path) noexcept;

/*!
 *  \brief 返回路径的文件名
 *         "/tmp/archive.tar.gz" -> "archive.tar.gz"
 * 
 *  \note  由于Unix-Like 中 "\"可以作为文件名, 故在此类平台中不支持Windows, UNC风格的路径.
 */
UTILITY_FUNCT_DECL std::string  path_find_filename(const std::string& path) noexcept;
UTILITY_FUNCT_DECL std::wstring path_find_filename(const std::wstring& path) noexcept;

/*!
 *  \brief 返回路径的文件名(不含扩展名)
 *         "/tmp/archive.tar.gz" -> "archive"
 * 
 *  \note  由于Unix-Like 中 "\"可以作为文件名, 故在此类平台中不支持Windows, UNC风格的路径.
 */
UTILITY_FUNCT_DECL std::string  path_find_basename(const std::string& path) noexcept;
UTILITY_FUNCT_DECL std::wstring path_find_basename(const std::wstring& path) noexcept;

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
UTILITY_FUNCT_DECL std::string  path_find_extension(const std::string& path, int flags = find_default) noexcept;
UTILITY_FUNCT_DECL std::wstring path_find_extension(const std::wstring& path, int flags = find_default) noexcept;

/*!
 *  \brief 文件名裁剪, 使文件名合法化
 *         nul          -> _nul
 *         aux          -> _aux
 *         read/me.txt  -> readme.txt
 *         readme.?txt  -> readme.txt
 *  
 *  \param filename 文件名, 不能为全路径或相对路径.
 * 
 *  \note  主要针对Windows平台, Unix-Like 平台对文件名的限制比较宽松, 只要不包含目录分隔符'/'即可
 *         但根据短板效应, 为了便于文件跨平台存储, 保持与Windows相同的限制规则.
 */
UTILITY_FUNCT_DECL std::string  path_filename_trim(const std::string& filename) noexcept;
UTILITY_FUNCT_DECL std::wstring path_filename_trim(const std::wstring& filename) noexcept;

/*!
 *  \brief 返回一个递增后的文件名
 *         etc. log.txt -> log(1).txt
 *                         log(1).txt -> log(2).txt
 */
UTILITY_FUNCT_DECL std::string  path_filename_increment(const std::string& filename) noexcept;
UTILITY_FUNCT_DECL std::wstring path_filename_increment(const std::wstring& filename) noexcept;

//! 
//! windows 方面的扩展
//! 
#if OS_WIN

/*!
 *  \brief 获得系统路径
 *  
 *  \param flag 定义在shlobj.h line 1204, 如:
 *              1. CSIDL_DESKTOP            表示系统桌面路径
 *              2. CSIDL_COMMON_APPDATA     表示%ProgramData%
 *              3. CSIDL_PROGRAM_FILES      表示C:\Program Files
 *              4. ...
 */
UTILITY_FUNCT_DECL fpath path_from_sysdir(int flag = 0);
UTILITY_FUNCT_DECL fpath path_from_sysdir(int flag, ferror& ferr) noexcept;

/*!
 *  \brief 在explorer中打开文件夹并选择指定的文件
 *
 *  \note  目前仅windows平台
 */
UTILITY_FUNCT_DECL void path_open_with_explorer(const fpath& path, bool select = true);
UTILITY_FUNCT_DECL void path_open_with_explorer(const fpath& path, bool select, ferror& ferr) noexcept;

#endif // OS_WIN

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/path_util.ipp"
#endif

#endif // path_util_h__
