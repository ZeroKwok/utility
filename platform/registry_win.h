#ifndef registry_win_h__
#define registry_win_h__

/*
*   registry_win.hpp
*
*   v0.1 2018-08 by GuoJH
*   v0.2 2020-11 by GuoJH
*/

#include <vector>
#include <utility.hpp>
#include <string/tstring.h>
#include <common/bytedata.hpp>
#include <platform/platform_cfg.h>
#include <platform/platform_error.h>

#ifndef OS_WIN
#   error registry_win.h  only supports Windows.
#endif

namespace util {
namespace win {

/*!
 *  /brief  注册表中键值的数据类型
 *  
 *  /see    https://docs.microsoft.com/en-us/windows/win32/sysinfo/registry-value-types
 */
enum registry_value_types
{
    registr_none,
    registr_dword,          //!< 4字节整数
    registr_qword,          //!< 8字节整数
    registr_binary,         //!< 二进制
    registr_string,         //!< 字符串
    registr_multi_string,   //!< 字符串数组
    registr_expand_string,  //!< 扩展字符串, 包含未扩展的环境变量, 如:%PATH%
};

/*!
 *  /brief  返回注册表指定的路径(subkey)是否存在.
 *
 *  /param  path   访问的注册表路径, 可以是一个预定义的HKEY开头, 如: HKEY_CURRENT_USER\Software\Wow6432Node.
 *                 若不包含预定义KHEY则默认访问 HKEY_LOCAL_MACHINE.
 *  /param  access 表示注册表附加访问属性, 若在64位操作系统下的32位程序要访问64位的注册表可以指定KEY_WOW64_64KEY.
 *  /param  error  若发生错误则会设置到该变量.
 * 
 *  /return true 表示指定的路径存在.
 */
UTILITY_FUNCT_DECL bool registry_path_exist(
    const tstring path, int access);
UTILITY_FUNCT_DECL bool registry_path_exist(
    const tstring path, int access, platform_error& error);

/*!
 *  /brief  返回注册表指定的值是否存在.
 * 
 *  /param path   略.
 *  /param name   表示注册表值名(value name).
 */
UTILITY_FUNCT_DECL bool registry_value_exist(
    const tstring path, const tstring& name, int access);
UTILITY_FUNCT_DECL bool registry_value_exist(
    const tstring path, const tstring& name, int access, platform_error& error);

/// 删除注册表目录树(多级目录), 及其下的值.
UTILITY_FUNCT_DECL void registry_tree_remove(
    const tstring path);
UTILITY_FUNCT_DECL void registry_tree_remove(
    const tstring path, platform_error& error);

/*!
 *  /brief  删除注册表目录(subkey), 及其下的值.
 *
 *  /note   不能删除多级目录, 否则将会得到: ERROR_ACCESS_DENIED.
 */
UTILITY_FUNCT_DECL void registry_directory_remove(
    const tstring path, int access);
UTILITY_FUNCT_DECL void registry_directory_remove(
    const tstring path, int access, platform_error& error);

/// 删除注册表中指定路径下的值.
UTILITY_FUNCT_DECL void registry_value_remove(
    const tstring path, const tstring& name, int access);
UTILITY_FUNCT_DECL void registry_value_remove(
    const tstring path, const tstring& name, int access, platform_error& error);

 /// 返回注册表中指定路径下值的类型.
UTILITY_FUNCT_DECL registry_value_types registry_get_type(
    const tstring path, const tstring& name, int access);
UTILITY_FUNCT_DECL registry_value_types registry_get_type(
    const tstring path, const tstring& name, int access, platform_error& error);

/*!
 *  /brief  返回值注册表中指定路径下4字节类型值的内容.
 *
 *  /note   若指定值的类型不为dword或不能转到dword, 将得到ERROR_UNSUPPORTED_TYPE.
 */
UTILITY_FUNCT_DECL int32_t registry_get_dword(
    const tstring path, const tstring& name, int access);
UTILITY_FUNCT_DECL int32_t registry_get_dword(
    const tstring path, const tstring& name, int access, platform_error& error);

/// 返回值注册表中指定路径下8字节类型值的内容.
UTILITY_FUNCT_DECL int64_t registry_get_qword(
    const tstring path, const tstring& name, int access);
UTILITY_FUNCT_DECL int64_t registry_get_qword(
    const tstring path, const tstring& name, int access, platform_error& error);

/// 返回值注册表中指定路径下二进制类型值的内容.
UTILITY_FUNCT_DECL bytedata registry_get_binary(
    const tstring path, const tstring& name, int access);
UTILITY_FUNCT_DECL bytedata registry_get_binary(
    const tstring path, const tstring& name, int access, platform_error& error);

/// 返回值注册表中指定路径下字符串类型值的内容.
UTILITY_FUNCT_DECL std::wstring registry_get_wstring(
    const tstring path, const tstring& name, int access);
UTILITY_FUNCT_DECL std::wstring registry_get_wstring(
    const tstring path, const tstring& name, int access, platform_error& error);

/// 返回值注册表中指定路径下扩展字符串类型值的内容(其中包含的环境变量将不被展开原样返回).
UTILITY_FUNCT_DECL std::wstring registry_get_expand_wstring(
    const tstring path, const tstring& name, int access);
UTILITY_FUNCT_DECL std::wstring registry_get_expand_wstring(
    const tstring path, const tstring& name, int access, platform_error& error);

/// 返回值注册表中指定路径下字符串数组类型值的内容.
UTILITY_FUNCT_DECL std::vector<std::wstring> registry_get_multi_wstring(
    const tstring path, const tstring& name, int access);
UTILITY_FUNCT_DECL std::vector<std::wstring> registry_get_multi_wstring(
    const tstring path, const tstring& name, int access, platform_error& error);

/// 设置4字节数据类型
UTILITY_FUNCT_DECL void registry_set_dword(
    const tstring path, const tstring& name, int32_t value, int access);
UTILITY_FUNCT_DECL void registry_set_dword(
    const tstring path, const tstring& name, int32_t value, int access, platform_error& error);

/// 设置8字节数据类型
UTILITY_FUNCT_DECL void registry_set_qword(
    const tstring path, const tstring& name, int64_t value, int access);
UTILITY_FUNCT_DECL void registry_set_qword(
    const tstring path, const tstring& name, int64_t value, int access, platform_error& error);

/// 设置二进制数据类型
UTILITY_FUNCT_DECL void registry_set_binary(
    const tstring path, const tstring& name, const bytedata& value, int access);
UTILITY_FUNCT_DECL void registry_set_binary(
    const tstring path, const tstring& name, const bytedata& value, int access, platform_error& error);

/// 设置字符串数据类型
UTILITY_FUNCT_DECL void registry_set_wstring(
    const tstring path, const tstring& name, const std::wstring& value, int access);
UTILITY_FUNCT_DECL void registry_set_wstring(
    const tstring path, const tstring& name, const std::wstring& value, int access, platform_error& error);

/// 设置字符串数组数据类型
UTILITY_FUNCT_DECL void registry_set_multi_wstring(
    const tstring path, const tstring& name, const std::vector<std::wstring>& value, int access);
UTILITY_FUNCT_DECL void registry_set_multi_wstring(
    const tstring path, const tstring& name, const std::vector<std::wstring>& value, int access, platform_error& error);

/// 获得指定路径下的所有顶级key
UTILITY_FUNCT_DECL std::vector<std::wstring> registry_get_wkeys(
    const tstring path, int access);
UTILITY_FUNCT_DECL std::vector<std::wstring> registry_get_wkeys(
    const tstring path, int access, platform_error& error);

/// 获得指定路径下所有的值
/// std::vector<std::pair<value name, value_types>>
UTILITY_FUNCT_DECL std::vector<std::pair<std::wstring, registry_value_types>>
    registry_get_wvalues(const tstring path, int access);
UTILITY_FUNCT_DECL std::vector<std::pair<std::wstring, registry_value_types>>
    registry_get_wvalues(const tstring path, int access, platform_error& error);

} // win
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/registry_win.ipp"
#endif

#endif // registry_win_h__
