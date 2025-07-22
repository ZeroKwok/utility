// This file is part of the utility project.
// Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.
//
// v0.1 2018-03
// v0.2 2019-07
// v0.3 2021-01
// v0.4 2024-06
// v0.5 2025-06

#ifndef filesystem_path_h__
#define filesystem_path_h__

#include "utility/config.h"
#include <filesystem>
#include <system_error>

#ifdef OS_WIN
#   include <shlobj.h>
#   include <windows.h>
#   include <Knownfolders.h>
#endif

#ifdef UTILITY_SUPPORT_QT
#   include <QString>
#endif

namespace UTILITY_NAMESPACE {
namespace fs {

// 导入 std::filesystem;
using std::filesystem::path;
using std::filesystem::absolute;
using std::filesystem::relative;
using std::filesystem::proximate;
using std::filesystem::canonical;
using std::filesystem::weakly_canonical;
using std::filesystem::equivalent;
using std::filesystem::current_path;
using std::filesystem::temp_directory_path;

/*!
 *  \brief 从 UTF-8 编码的字符串构造文件路径
 */
UTILITY_API path path_from_utf8(const std::string& str);

#ifdef UTILITY_SUPPORT_QT
/*!
 *  \brief 从 QString 字符串构造文件路径
 */
UTILITY_API path path_from(const QString& str);
#endif // UTILITY_SUPPORT_QT

/*!
 *  \brief 返回模块目录的路径工厂函数
 * 
 *  \param module 模块实例id, 0代表当前模块.
 *  \param stems 可以是文件名, 也可以是目录名, 附加在结果路径之后.
 *  
 *  \note  在windows中 module 代表的是模块句柄(HMODULE),
 *         而linux  中 module 代表的是进程id(pid_t)
 *         在不支持 /proc 文件系统的平台, path_from_module(), path_from_module_dir() 返回程序的当前目录.
 */
UTILITY_API path path_from_module(intptr_t module = 0);
UTILITY_API path path_from_module(intptr_t module, std::error_code& error) noexcept;
UTILITY_API path path_from_module_dir(intptr_t module = 0);
UTILITY_API path path_from_module_dir(intptr_t module, std::error_code& error) noexcept;

/*!
 *  \brief 返回模块目录的便捷API
 *  \note  相当于: path_append(path_from_module_dir(0, ferr), stems);
 */
UTILITY_API path path_from_module_dir(intptr_t module, const path& stems);
UTILITY_API path path_from_module_dir(intptr_t module, const path& stems, std::error_code& error) noexcept;

/*!
 *  \brief  返回系统的临时目录
 *  \note   std::filesystem::temp_directory_path()
 */
UTILITY_API path path_from_temp();
UTILITY_API path path_from_temp(std::error_code& error) noexcept;
UTILITY_API path path_from_temp(const path& stems);
UTILITY_API path path_from_temp(const path& stems, std::error_code& error) noexcept;

/*!
 *  \brief  返回进程所属用户的家目录
 *  \return 在 Windows 中相当于环境变量 "%USERPROFILE%", 而 Unix-Like 相当于 "~".
 */
UTILITY_API path path_from_home();
UTILITY_API path path_from_home(std::error_code& error) noexcept;
UTILITY_API path path_from_home(const path& stems);
UTILITY_API path path_from_home(const path& stems, std::error_code& error) noexcept;

/*!
 *  \brief 判断路径是否可写
 *  \note  若指定路径不存在, 则向上查找第一个存在的父目录并判断是否可写.
 *         函数内部会尝试向目标目录写入文件并删除来测试是否具有写权限.
 */
UTILITY_API bool path_is_writable(const path& path);
UTILITY_API bool path_is_writable(const path& path, std::error_code& error) noexcept;

/*!
 *  \brief 文件名合法化处理
 *
 *  该函数用于将路径中的文件名部分裁剪并合法化，确保其符合跨平台（特别是 Windows）对文件名的限制。
 *  将保留路径中的目录结构，仅处理最后一级文件名中的非法字符。
 *
 *  常见示例：
 *    "nul"            -> "(nul)"
 *    "aux"            -> "(aux)"
 *    "read/me.txt"    -> "readme.txt"
 *    "readme.?txt"    -> "readme.txt"
 *
 *  若 placeholder 为 "."，则：
 *    "read/me.txt"    -> "read.me.txt"
 *    "readme.?txt"    -> "readme..txt"
 *
 *  \param filename        要合法化处理的完整路径，函数将保留其父目录，仅替换文件名中的非法字符。
 *  \param placeholder     占位符，用于替换非法字符。传空字符串 "" 则表示直接删除非法字符。
 *
 *  \note
 *    - 仅处理路径中最后一级文件名（即路径的 `filename()` 部分）
 *    - 对于 Windows 特有的非法文件名（如 nul, aux, con, prn, lpt1 等）进行保护性括号包装
 *    - 非法字符包括（但不限于）：`<>:"/\\|?*` 及控制字符 (ASCII < 32)
 *    - 虽主要参考 Windows 限制，但建议跨平台应用中也使用此规范处理文件名
 *
 *  \return 一个路径对象，保留原始路径的目录部分，但文件名已合法化处理。
 */
UTILITY_API path path_filename_trim(const path& filename, const std::string& placeholder = "") noexcept;

/*!
 *  \brief 生成递增版本的文件名（自动编号）
 *
 *  该函数返回一个在原始文件名基础上添加递增编号的路径，常用于自动保存、避免覆盖等场景。
 *  函数仅修改路径中的文件名部分，保留原始目录结构。
 *
 *  示例（ignore_extension = false）：
 *      "log.txt"     -> "log(1).txt"
 *      "log(1).txt"  -> "log(2).txt"
 *
 *  示例（ignore_extension = true）：
 *      "log.txt"     -> "log.txt(1)"
 *
 *  \param filename           要处理的文件路径，仅最后一级文件名受影响。
 *  \param ignore_extension   是否忽略扩展名，默认为 false。
 *                            若为 true，则编号附加在完整文件名之后（包括扩展名）。
 *
 *  \note
 *    - 该函数不检查生成的新文件名是否已存在，仅用于构造名称。
 *    - 若原始文件名中已包含 "(n)" 格式，将自动递增。
 *    - 文件名中多个 "(n)" 格式将仅识别最末尾合法部分。
 *
 *  \return 递增后的完整路径，目录结构不变，仅文件名发生变化。
 */
UTILITY_API path path_filename_increment(const path& filename, bool ignore_extension = false) noexcept;

//
// windows 方面的扩展
//
#if OS_WIN

namespace win {

/*!
 *  \brief 获得系统路径, 弃用, 建议使用 KnownFolders 版本代替
 *  
 *  \param flag 定义在shlobj.h line 1204, 如:
 *              1. CSIDL_DESKTOP            表示系统桌面路径
 *              2. CSIDL_COMMON_APPDATA     表示%ProgramData%
 *              3. CSIDL_PROGRAM_FILES      表示C:\Program Files
 *              4. ...
 *  \see   https://docs.microsoft.com/en-us/windows/win32/shell/csidl
 *  \note  标记为virtual folder的CSIDL, 可能会失败.
 *         如: CSIDL_PERSONAL, 在某些环境下面将得到ERROR_ACCESS_DENIED,
 *         因此该方法总是应该判断错误，而不是忽略错误，否则抛出异常将导致致命错误。
 */
UTILITY_API path __DEPRECATED path_from_sysdir(int flag = 0);
UTILITY_API path __DEPRECATED path_from_sysdir(int flag, std::error_code& error) noexcept;

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
/*!
 *  \brief 获得系统路径(建议使用)
 *
 *  \param rfid 定义在KnownFolders.h, 如:
 *              1. FOLDERID_Desktop         表示系统桌面路径
 *              2. FOLDERID_ProgramData     表示%ProgramData%
 *              3. FOLDERID_ProgramFiles    表示C:\Program Files
 *              4. ...
 *  \see   https://docs.microsoft.com/en-us/windows/win32/shell/knownfolderid
 */
UTILITY_API path path_from_sysdir(REFKNOWNFOLDERID rfid = FOLDERID_Desktop);
UTILITY_API path path_from_sysdir(REFKNOWNFOLDERID rfid, std::error_code& error) noexcept;
#endif

/*!
 *  \brief 在explorer中打开文件夹并选择指定的文件
 *
 *  \note  目前仅windows平台
 */
UTILITY_API void path_open_with_explorer(const path& path, bool select = true);
UTILITY_API void path_open_with_explorer(const path& path, bool select, std::error_code& error) noexcept;

} // win

#endif // OS_WIN

} // fs
} // UTILITY_NAMESPACE

#endif // filesystem_path_h__
