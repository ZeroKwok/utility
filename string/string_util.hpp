#ifndef string_util_h__
#define string_util_h__

/*
*   string_util.hpp 
*
*   v0.1  _2018-04 By GuoJH
*   v0.2  _2020-08 By GuoJH
*/

#include <string>
#include <utility.hpp>

namespace util {

/*
*   宽字节, 多字节字符版本的格式化
*/
UTILITY_FUNCT_DECL std::string sformat(const char * format, ...);
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
*	大小写转换(弃用)
*/
UTILITY_FUNCT_DECL std::string  tolower(const std::string& str);
UTILITY_FUNCT_DECL std::wstring tolower(const std::wstring& str);
UTILITY_FUNCT_DECL std::string  toupper(const std::string& str);
UTILITY_FUNCT_DECL std::wstring toupper(const std::wstring& str);

/*
*   字符串二分割(弃用)
*/
UTILITY_FUNCT_DECL std::pair<std::string, std::string>
    right_split(const std::string& target, const std::string& search);
UTILITY_FUNCT_DECL std::pair<std::wstring, std::wstring>
    right_split(const std::wstring& target, const std::wstring& search);
UTILITY_FUNCT_DECL std::pair<std::string, std::string>
    left_split(const std::string& target, const std::string& search);
UTILITY_FUNCT_DECL std::pair<std::wstring, std::wstring>
    left_split(const std::wstring& target, const std::wstring& search);

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/string_util.ipp"
#endif

#endif // string_util_h__
