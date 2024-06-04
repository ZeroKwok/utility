// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.
//
// v0.1  2018-04
// v0.2  2020-12
// v0.3  2024-06

#ifndef string_conv_h__
#define string_conv_h__

#include "config.h"
#include "string_util.h"

#ifdef UTILITY_SUPPORT_QT
#   include <QString>
#endif

namespace UTILITY_NAMESPACE {

//! @brief local 8 bit string or std::string to std::wstring.
UTILITY_API std::wstring& string_to_wstring(
    const std::string& input, std::wstring& output);

//! @brief std::wstring to local 8 bit string or std::string.
UTILITY_API std::string& wstring_to_string(
    const std::wstring& input, std::string& output);

//! @brief utf8 string to std::wstring.
UTILITY_API std::wstring& utf8_to_wstring(
    const std::string& input, std::wstring& output);

//! @brief std::wstring to utf8 string.
UTILITY_API std::string& wstring_to_utf8(
    const std::wstring& input, std::string& output);

//! @brief utf8 string to local 8 bit string or std::string.
//! On Linux, no conversion is done, because UTF-8 is the default.
UTILITY_API std::string& utf8_to_string(
    const std::string& input, std::string& output);

//! @brief local 8 bit string or std::string to utf8 string.
//! On Linux, no conversion is done, because UTF-8 is the default.
UTILITY_API std::string& string_to_utf8(
    const std::string& input, std::string& output);

//
// 便捷 API
//
UTILITY_API std::string  str(const std::wstring& string);
UTILITY_API std::string  str_u8(const std::string& string);
UTILITY_API std::string  utf8(const std::wstring& string);
UTILITY_API std::string  utf8(const std::string& string);
UTILITY_API std::wstring wstr(const std::string& string);
UTILITY_API std::wstring wstr_u8(const std::string& string);

#ifdef UTILITY_SUPPORT_QT

UTILITY_API std::string  str (const QString& string);
UTILITY_API std::string  utf8(const QString& string);
UTILITY_API std::wstring wstr(const QString& string);

UTILITY_API QString qstr(const std::string& string);
UTILITY_API QString qstr(const std::wstring& string);
UTILITY_API QString qstr_u8(const std::wstring& string);

#endif // UTILITY_SUPPORT_QT

} // namespace UTILITY_NAMESPACE

#endif // string_conv_h__