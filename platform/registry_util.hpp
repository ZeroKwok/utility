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
 *  /brief  ע����ֵ��������
 *  
 *  /see    https://docs.microsoft.com/en-us/windows/win32/sysinfo/registry-value-types
 */
enum registry_value_types
{
    registr_none,
    registr_dword,          //!< 4�ֽ�����
    registr_qword,          //!< 8�ֽ�����
    registr_binary,         //!< ������
    registr_string,         //!< �ַ���
    registr_multi_string,   //!< �ַ�������
    registr_expand_string,  //!< ��չ�ַ���, ����δ��չ�Ļ�������, ��:%PATH%
};

/// ����ֵע���·��(subkey)�Ƿ����
UTILITY_FUNCT_DECL bool registry_path_exist(
    const tstring path, int access, platform_error& error);

/*!
 *  /brief  ����ע���ֵ�Ƿ����.
 * 
 *  /param path   ���ʵ�ע���·��, ������һ��Ԥ�����HKEY��ͷ, ��: HKEY_CURRENT_USER\Software\Wow6432Node.
 *                ��������Ԥ����KHEY��Ĭ�Ϸ��� HKEY_LOCAL_MACHINE.
 *  /param name   ��ʾע���ֵ��(value name).
 *  /param access ��ʾע����ӷ�������, ����64λ����ϵͳ�µ�32λ����Ҫ����64λ��ע������ָ��KEY_WOW64_64KEY.
 * 
 */
UTILITY_FUNCT_DECL bool registry_value_exist(
    const tstring path, const tstring& name, int access, platform_error& error);

 /// ����ע���ֵ����.
UTILITY_FUNCT_DECL registry_value_types registry_get_type(
    const tstring path, const tstring& name, int access, platform_error& error);

/// ����ֵ��4�ֽ�����
UTILITY_FUNCT_DECL int32_t registry_get_dword(
    const tstring path, const tstring& name, int access, platform_error& error);

/// ����ֵ��8�ֽ�����
UTILITY_FUNCT_DECL int64_t registry_get_qword(
    const tstring path, const tstring& name, int access, platform_error& error);

/// ����ֵ�Ķ���������
UTILITY_FUNCT_DECL bytedata registry_get_binary(
    const tstring path, const tstring& name, int access, platform_error& error);

/// ����ֵ���ַ�������
UTILITY_FUNCT_DECL std::wstring registry_get_wstring(
    const tstring path, const tstring& name, int access, platform_error& error);

/// ����ֵ����չ�ַ�������, ������������չ��, ԭ������.
UTILITY_FUNCT_DECL std::wstring registry_get_expand_wstring(
    const tstring path, const tstring& name, int access, platform_error& error);

/// ����ֵ���ַ�����������
UTILITY_FUNCT_DECL std::vector<std::wstring> registry_get_multi_wstring(
    const tstring path, const tstring& name, int access, platform_error& error);

/// ����4�ֽ���������
UTILITY_FUNCT_DECL void registry_set_dword(
    const tstring path, const tstring& name, int32_t value, int access, platform_error& error);

/// ����8�ֽ���������
UTILITY_FUNCT_DECL void registry_set_qword(
    const tstring path, const tstring& name, int64_t value, int access, platform_error& error);

/// ���ö�������������
UTILITY_FUNCT_DECL void registry_set_binary(
    const tstring path, const tstring& name, const bytedata& value, int access, platform_error& error);

/// �����ַ�����������
UTILITY_FUNCT_DECL void registry_set_wstring(
    const tstring path, const tstring& name, const std::wstring& value, int access, platform_error& error);

/// �����ַ���������������
UTILITY_FUNCT_DECL void registry_set_multi_wstring(
    const tstring path, const tstring& name, const std::vector<std::wstring>& value, int access, platform_error& error);


} // win
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/registry_util.ipp "
#endif

#endif // registry_util_h__
