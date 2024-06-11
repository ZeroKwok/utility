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

#ifndef filesystem_path_h__
#define filesystem_path_h__

#include "error.h"
#include <filesystem>

#ifdef OS_WIN
#   include <shlobj.h>
#   include <windows.h>
#   include <Knownfolders.h>
#endif

#ifdef UTILITY_SUPPORT_QT
#   include <QString>
#endif // UTILITY_SUPPORT_QT

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
 *  \brief 返回一个递增后的文件名
 *         etc. log.txt -> log(1).txt
 *                         log(1).txt -> log(2).txt
 *  \param ignore_extension 是否忽略扩展名: log.txt -> log.txt(1)
 *  \note 不检测递增后的文件名是否存在, 仅实现文件名的递增.
 */
UTILITY_API path filename_increment(const path& path, bool ignore_extension = false) noexcept;

/*!
 *  \brief 文件名裁剪, 使文件名合法化
 *         nul          -> (nul)
 *         aux          -> (aux)
 *         read/me.txt  -> readme.txt
 *         readme.?txt  -> readme.txt
 *      若 placeholder 为 ".", 则:
 *         nul          -> (nul)
 *         aux          -> (aux)
 *         read/me.txt  -> read.me.txt
 *         readme.?txt  -> readme..txt
 * 
 *  \param filename 文件名, 必须是文件名, 否则将路径分隔符作为文件名中的非法字符处理.
 *  \param placeholder 占位符, 用于替换非法字符, "" 空字符则表示删除非法字符.
 * 
 *  \note  主要针对 Windows 平台, Unix-Like 平台对文件名的限制比较宽松, 只要不包含目录分隔符 '/' 即可
 *         但为了便于文件跨平台存储, 保持与 Windows 相同的限制规则.
 */
UTILITY_API std::string  filename_trim(const std::string& filename, const std::string& placeholder = "") noexcept;
UTILITY_API std::wstring filename_trim(const std::wstring& filename, const std::wstring& placeholder = L"") noexcept;

/*!
 *  \brief 文件名裁剪, 使文件名合法化
 *  \note  同字符串版类似，唯一区别, 在于仅将最后一个路径分隔符之后的内容视为文件名.
 */
UTILITY_API path path_filename_trim(const path& path, const std::string& placeholder = "") noexcept;

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
