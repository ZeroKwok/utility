// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#ifndef string_util_h__
#define string_util_h__

#include "config.h"
#include <string>

namespace UTILITY_NAMESPACE {

/*!
 *   /brief 宽字节, 多字节字符版本的格式化
 *   /note 弃用, 请选择 std::format 或 fmt::format
 */
UTILITY_API std::string  sformat(const char * format, ...);
UTILITY_API std::wstring sformat(const wchar_t * format, ...);

/*!
 *   /brief 字符串替换
 */
UTILITY_API std::string& replace(
    std::string& target,
    const std::string& before,
    const std::string& after);
UTILITY_API std::wstring& replace(
    std::wstring& target,
    const std::wstring& before,
    const std::wstring& after);
UTILITY_API std::string replace_copy(
    const std::string& target,
    const std::string& before,
    const std::string& after);
UTILITY_API std::wstring replace_copy(
    const std::wstring& target,
    const std::wstring& before,
    const std::wstring& after);

/*!
 *   /brief 大小写转换
 */
UTILITY_API std::string  to_lower(const std::string& str);
UTILITY_API std::wstring to_lower(const std::wstring& str);
UTILITY_API std::string  to_upper(const std::string& str);
UTILITY_API std::wstring to_upper(const std::wstring& str);

/*!
 *  /brief  在给定的目标字符串从左边截取一段子串并返回;
 *  /param  target  给定的目标字符串;
 *  /param  mark    给定的标记;
 *  /return         返回从mark标记处(不包含该标记)的左边截取字符串, 若给定的标记不存在则返回空字符串;
 */
UTILITY_API std::string  left(
    const std::string& target,
    const std::string& mark);
UTILITY_API std::wstring left(
    const std::wstring& target,
    const std::wstring& mark);

/*!
 *  /brief  在给定的目标字符串从右边截取一段子串并返回;
 *  /param  target  给定的目标字符串;
 *  /param  mark    给定的标记;
 *  /return         返回从mark标记处(不包含该标记)的右边截取字符串, 若给定的标记不存在则返回空字符串;
 */
UTILITY_API std::string  right(
    const std::string& target,
    const std::string& mark);
UTILITY_API std::wstring right(
    const std::wstring& target,
    const std::wstring& mark);

/*!
 *  /brief  给定标记, 从左起将目标字符串分成两半
 *  /param  target 目标字符串
 *  /param  mark   若target字符串中存在mark指定的标记则从该位置分割字符串.
 *  /return        若存在mark指定的标记, 则返回分割结果: {left, right}, 否则返回: {}.
 *                 需要注意的是该结果不包含mark.
 */
UTILITY_API std::pair<std::string, std::string> in_half_from_left(
    const std::string& target, const std::string& mark);
UTILITY_API std::pair<std::wstring, std::wstring> in_half_from_left(
    const std::wstring& target, const std::wstring& mark);

UTILITY_API std::pair<std::string, std::string> in_half_from_right(
    const std::string& target, const std::string& mark);
UTILITY_API std::pair<std::wstring, std::wstring> in_half_from_right(
    const std::wstring& target, const std::wstring& mark);

enum between_policy
{
    without_mark  = 0x00, //!< between() 返回的子串不包含标记内容;
    contains_mark = 0x01, //!< between() 返回的子串包含标记内容;
};

/*!
 *  /brief  在给定的目标字符串中间截取一段子串并返回;
 *  /param  target  给定的目标字符串;
 *  /param  left    给定的坐标记;
 *  /param  right   给定的右标记;
 *  /param  policy  给定的截取策略, 若为without_mark则不包含left与right本身, 否则包含它;
 *  /return         返回从left~right 之间间截取字符串, 若给定的标记不存在则返回空字符串;
 */
UTILITY_API std::string between(
    const std::string& target, 
    const std::string& left, 
    const std::string& right,
    between_policy policy = without_mark);
UTILITY_API std::wstring between(
    const std::wstring& target,
    const std::wstring& left,
    const std::wstring& right,
    between_policy policy = without_mark);

/*!
 *  /brief  判断目标字符串是否以指定内容作为开头;
 *  /param  target  给定的目标字符串;
 *  /param  head    给定的字符串;
 *  /return         成立返回true, 否者false;
 */
UTILITY_API bool start_with(const std::string& target, const std::string& head);
UTILITY_API bool start_with(const std::wstring& target, const std::wstring& head);

/*!
 *  /brief  判断目标字符串是否以指定内容作为结束;
 *  /param  target  给定的目标字符串;
 *  /param  tail    给定的匹配字符串;
 *  /return         成立返回true, 否者false;
 */
UTILITY_API bool end_with(const std::string& target, const std::string& tail);
UTILITY_API bool end_with(const std::wstring& target, const std::wstring& tail);

} // namespace UTILITY_NAMESPACE

#endif // string_util_h__