#ifndef registry_util_h__
#define registry_util_h__

/*
*   registry_util.hpp 
*
*   v0.1 2018-08 by GuoJH
*/

#include <vector>
#include <string/tstring.hpp>
#include <common/bytedata.hpp>
#include <platform/platform_error.hpp>

namespace util {
namespace win {

/*!
 *  /brief  注册表键值数据类型
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

/// 返回值注册表路径(subkey)是否存在
UTILITY_FUNCT_DECL bool registry_path_exist(
    const tstring path, int access, platform_error& error);

/*!
 *  /brief  返回注册表值是否存在.
 * 
 *  /param path   访问的注册表路径, 可以是一个预定义的HKEY开头, 如: HKEY_CURRENT_USER\Software\Wow6432Node.
 *                若不包含预定义KHEY则默认访问 HKEY_LOCAL_MACHINE.
 *  /param name   表示注册表值名(value name).
 *  /param access 表示注册表附加访问属性, 若在64位操作系统下的32位程序要访问64位的注册表可以指定KEY_WOW64_64KEY.
 * 
 */
UTILITY_FUNCT_DECL bool registry_value_exist(
    const tstring path, const tstring& name, int access, platform_error& error);

 /// 返回注册表值类型.
UTILITY_FUNCT_DECL registry_value_types registry_get_type(
    const tstring path, const tstring& name, int access, platform_error& error);

/// 返回值的4字节类型
UTILITY_FUNCT_DECL int32_t registry_get_dword(
    const tstring path, const tstring& name, int access, platform_error& error);

/// 返回值的8字节类型
UTILITY_FUNCT_DECL int64_t registry_get_qword(
    const tstring path, const tstring& name, int access, platform_error& error);

/// 返回值的二进制类型
UTILITY_FUNCT_DECL bytedata registry_get_binary(
    const tstring path, const tstring& name, int access, platform_error& error);

/// 返回值的字符串类型
UTILITY_FUNCT_DECL std::wstring registry_get_wstring(
    const tstring path, const tstring& name, int access, platform_error& error);

/// 返回值的扩展字符串类型, 即环境变量不展开, 原样返回.
UTILITY_FUNCT_DECL std::wstring registry_get_expand_wstring(
    const tstring path, const tstring& name, int access, platform_error& error);

/// 返回值的字符串数组类型
UTILITY_FUNCT_DECL std::vector<std::wstring> registry_get_multi_wstring(
    const tstring path, const tstring& name, int access, platform_error& error);

/// 设置4字节数据类型
UTILITY_FUNCT_DECL void registry_set_dword(
    const tstring path, const tstring& name, int32_t value, int access, platform_error& error);

/// 设置8字节数据类型
UTILITY_FUNCT_DECL void registry_set_qword(
    const tstring path, const tstring& name, int64_t value, int access, platform_error& error);

/// 设置二进制数据类型
UTILITY_FUNCT_DECL void registry_set_binary(
    const tstring path, const tstring& name, const bytedata& value, int access, platform_error& error);

/// 设置字符串数据类型
UTILITY_FUNCT_DECL void registry_set_wstring(
    const tstring path, const tstring& name, const std::wstring& value, int access, platform_error& error);

/// 设置字符串数组数据类型
UTILITY_FUNCT_DECL void registry_set_multi_wstring(
    const tstring path, const tstring& name, const std::vector<std::wstring>& value, int access, platform_error& error);


} // win
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/registry_util.ipp "
#endif

#endif // registry_util_h__
