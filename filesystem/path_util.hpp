#ifndef path_util_h__
#define path_util_h__

/*
*   path_util.hpp
*
*   v0.1 2018-3 by GuoJH
*   v0.2 2019-7 by GuoJH
*/

#include <filesystem/file_util.hpp>
#include <filesystem/filesystem_cfg.hpp>

namespace util {

/*
*   路径工厂函数
*/
UTILITY_FUNCT_DECL fpath path_from_module(intptr_t module = 0);
UTILITY_FUNCT_DECL fpath path_from_module(intptr_t module, ferror& ferr);
UTILITY_FUNCT_DECL fpath path_from_module_dir(intptr_t module = 0);
UTILITY_FUNCT_DECL fpath path_from_module_dir(intptr_t module, ferror& ferr);
UTILITY_FUNCT_DECL fpath path_from_module_dir(const fpath& stem);
UTILITY_FUNCT_DECL fpath path_from_module_dir(const fpath& stem, ferror& ferr);
UTILITY_FUNCT_DECL fpath path_from_temp();
UTILITY_FUNCT_DECL fpath path_from_temp(ferror& ferr);
UTILITY_FUNCT_DECL fpath path_from_sysdir(int flag = 0);
UTILITY_FUNCT_DECL fpath path_from_sysdir(int flag, ferror& ferr);

/*
*   在explorer中打开文件夹并选择指定的文件
*/
UTILITY_FUNCT_DECL void path_open_with_explorer(const fpath& path, bool select = true);
UTILITY_FUNCT_DECL void path_open_with_explorer(const fpath& path, bool select, ferror& ferr);

/*
*	判断是否为Root目录
*/
UTILITY_FUNCT_DECL bool path_is_root(const fpath& path);

/*
*	判断是否为UNC路径
*/
UTILITY_FUNCT_DECL bool path_is_unc(const fpath& path);

/*
*	判断是否为URL路径
*/
UTILITY_FUNCT_DECL bool path_is_url(const fpath& path);

/*
*	判断路径是否为Windows风格
*   1. 检测到驱动器号 :
*   2. 检测到分隔符   \
*/
UTILITY_FUNCT_DECL bool path_is_win_style(const fpath& path);

/*
*	判断路径是否为Linux风格
*   1. 检测到根目录 /
*   2. 检测到分隔符 / 且为 非Windows 风格
*/
UTILITY_FUNCT_DECL bool path_is_linux_style(const fpath& path);

/*
*	判断路径是否为UNC风格
*   1. 检测到前缀     \\
*   2. 未检测到分隔符 /
*/
UTILITY_FUNCT_DECL bool path_is_unc_style(const fpath& path);

/*
*	判断是否为远程路径 (不能判断断开连接的网络驱动器)
*/
UTILITY_FUNCT_DECL bool path_is_remote(const fpath& path);

/*
*	判断路径是否可写
*   若指定路径不存在, 则向上查找第一个存在的父目录并判断是否可写
*/
UTILITY_FUNCT_DECL bool path_is_writable(const fpath& path);
UTILITY_FUNCT_DECL bool path_is_writable(const fpath& path, ferror& ferr);

/*
*	路径处理
*/
UTILITY_FUNCT_DECL std::string  path_append(const std::string& path, const std::string& stem_1);
UTILITY_FUNCT_DECL std::wstring path_append(const std::wstring& path, const std::wstring& stem_1);
UTILITY_FUNCT_DECL std::string  path_append(const std::string& path, const std::string& stem_1, const std::string& stem_2);
UTILITY_FUNCT_DECL std::wstring path_append(const std::wstring& path, const std::wstring& stem_1, const std::wstring& stem_2);

/*
*	获得root目录
*   本地路径: 驱动器    C:\Program Files\windows    -> C:\
*   网络路径: UNC根目录 \\192.168.0.13\share\folder -> \\192.168.0.13\share
*/
UTILITY_FUNCT_DECL std::string  path_find_root(const std::string& path);
UTILITY_FUNCT_DECL std::wstring path_find_root(const std::wstring& path);
UTILITY_FUNCT_DECL std::string  path_find_parent(const std::string& path);
UTILITY_FUNCT_DECL std::wstring path_find_parent(const std::wstring& path);
UTILITY_FUNCT_DECL std::string  path_find_filename(const std::string& path);
UTILITY_FUNCT_DECL std::wstring path_find_filename(const std::wstring& path);
UTILITY_FUNCT_DECL std::string  path_find_extension(const std::string& path, bool hasdot = true, bool lower = true);
UTILITY_FUNCT_DECL std::wstring path_find_extension(const std::wstring& path, bool hasdot = true, bool lower = true);

/*
*   文件名裁剪, 使文件名合法化
*   filename 仅文件名, 不能为路径
*/
UTILITY_FUNCT_DECL std::string  path_filename_trim(const std::string& filename);
UTILITY_FUNCT_DECL std::wstring path_filename_trim(const std::wstring& filename);

/*
*   返回一个递增后的文件名
*   etc. log.txt -> log(1).txt
*                   log(1).txt -> log(2).txt
*/
UTILITY_FUNCT_DECL std::string  path_filename_increment(const std::string& filename);
UTILITY_FUNCT_DECL std::wstring path_filename_increment(const std::wstring& filename);

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/path_util.ipp"
#endif

#endif // path_util_h__
