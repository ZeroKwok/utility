#ifndef string_util_h__
#define string_util_h__

/*
*   string_util.hpp 
*
*   v0.1  2018-04 By GuoJH
*   v0.2  2020-08 By GuoJH
*/

#include <string>
#include <string/string_cfg.hpp>

namespace util {

/*
*   ���ֽ�, ���ֽ��ַ��汾�ĸ�ʽ��
*/
UTILITY_FUNCT_DECL std::string  sformat(const char * format, ...);
UTILITY_FUNCT_DECL std::wstring sformat(const wchar_t * format, ...);

/*
*   �ַ����滻
*/
UTILITY_FUNCT_DECL std::string& replace(
    std::string& target,
    const std::string& before,
    const std::string& after);
UTILITY_FUNCT_DECL std::wstring& replace(
    std::wstring& target,
    const std::wstring& before,
    const std::wstring& after);
UTILITY_FUNCT_DECL std::string replace_copy(
    const std::string& target,
    const std::string& before,
    const std::string& after);
UTILITY_FUNCT_DECL std::wstring replace_copy(
    const std::wstring& target,
    const std::wstring& before,
    const std::wstring& after);

/*
*	��Сдת��
*/
UTILITY_FUNCT_DECL std::string  to_lower(const std::string& str);
UTILITY_FUNCT_DECL std::wstring to_lower(const std::wstring& str);
UTILITY_FUNCT_DECL std::string  to_upper(const std::string& str);
UTILITY_FUNCT_DECL std::wstring to_upper(const std::wstring& str);

/*!
 *  /brief  �ڸ�����Ŀ���ַ�������߽�ȡһ���Ӵ�������;
 *  /param  target  ������Ŀ���ַ���;
 *  /param  mark    �����ı��;
 *  /return         ���ش�mark��Ǵ�(�������ñ��)����߽�ȡ�ַ���, �������ı�ǲ������򷵻ؿ��ַ���;
 */
UTILITY_FUNCT_DECL std::string  left(
    const std::string& target,
    const std::string& mark);
UTILITY_FUNCT_DECL std::wstring left(
    const std::wstring& target,
    const std::wstring& mark);

/*!
 *  /brief  �ڸ�����Ŀ���ַ������ұ߽�ȡһ���Ӵ�������;
 *  /param  target  ������Ŀ���ַ���;
 *  /param  mark    �����ı��;
 *  /return         ���ش�mark��Ǵ�(�������ñ��)���ұ߽�ȡ�ַ���, �������ı�ǲ������򷵻ؿ��ַ���;
 */
UTILITY_FUNCT_DECL std::string  right(
    const std::string& target,
    const std::string& mark);
UTILITY_FUNCT_DECL std::wstring right(
    const std::wstring& target,
    const std::wstring& mark);

enum UTILITY_CLASS_DECL between_policy
{
    without_mark = 0x00, ///< between()���ص��Ӵ��������������;
    contains_mark = 0x01, ///< between()���ص��Ӵ������������;
};

/*!
 *  /brief  �ڸ�����Ŀ���ַ����м��ȡһ���Ӵ�������;
 *  /param  target  ������Ŀ���ַ���;
 *  /param  left    �����������;
 *  /param  right   �������ұ��;
 *  /param  policy  �����Ľ�ȡ����, ��Ϊwithout_mark�򲻰���left��right����, ���������;
 *  /return         ���ش�left~right ֮����ȡ�ַ���, �������ı�ǲ������򷵻ؿ��ַ���;
 */
UTILITY_FUNCT_DECL std::string  between(
    const std::string& target, 
    const std::string& left, 
    const std::string& right,
    between_policy policy = without_mark);
UTILITY_FUNCT_DECL std::wstring between(
    const std::wstring& target,
    const std::wstring& left,
    const std::wstring& right,
    between_policy policy = without_mark);

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/string_util.ipp"
#endif

#endif // string_util_h__
