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
*   宽字节, 多字节字符版本的格式化
*/
UTILITY_FUNCT_DECL std::string  sformat(const char * format, ...);
UTILITY_FUNCT_DECL std::wstring sformat(const wchar_t * format, ...);

/*
*   字符串替换
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
*	大小写转换
*/
UTILITY_FUNCT_DECL std::string  to_lower(const std::string& str);
UTILITY_FUNCT_DECL std::wstring to_lower(const std::wstring& str);
UTILITY_FUNCT_DECL std::string  to_upper(const std::string& str);
UTILITY_FUNCT_DECL std::wstring to_upper(const std::wstring& str);

/*!
 *  /brief  在给定的目标字符串从左边截取一段子串并返回;
 *  /param  target  给定的目标字符串;
 *  /param  mark    给定的标记;
 *  /return         返回从mark标记处(不包含该标记)的左边截取字符串, 若给定的标记不存在则返回空字符串;
 */
UTILITY_FUNCT_DECL std::string  left(
    const std::string& target,
    const std::string& mark);
UTILITY_FUNCT_DECL std::wstring left(
    const std::wstring& target,
    const std::wstring& mark);

/*!
 *  /brief  在给定的目标字符串从右边截取一段子串并返回;
 *  /param  target  给定的目标字符串;
 *  /param  mark    给定的标记;
 *  /return         返回从mark标记处(不包含该标记)的右边截取字符串, 若给定的标记不存在则返回空字符串;
 */
UTILITY_FUNCT_DECL std::string  right(
    const std::string& target,
    const std::string& mark);
UTILITY_FUNCT_DECL std::wstring right(
    const std::wstring& target,
    const std::wstring& mark);

enum UTILITY_CLASS_DECL between_policy
{
    without_mark = 0x00, ///< between()返回的子串不包含标记内容;
    contains_mark = 0x01, ///< between()返回的子串包含标记内容;
};

/*!
 *  /brief  在给定的目标字符串中间截取一段子串并返回;
 *  /param  target  给定的目标字符串;
 *  /param  left    给定的坐标记;
 *  /param  right   给定的右标记;
 *  /param  policy  给定的截取策略, 若为without_mark则不包含left与right本身, 否则包含它;
 *  /return         返回从left~right 之间间截取字符串, 若给定的标记不存在则返回空字符串;
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
