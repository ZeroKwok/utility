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

#include <filesystem>
#ifdef UTILITY_SUPPORT_QT
#   include <QString>
#endif

namespace UTILITY_NAMESPACE {

/**
 * @brief Convert local 8-bit string or std::string to std::wstring.
 * @param input The input string to convert.
 * @param output The converted output std::wstring.
 * @return Reference to the converted std::wstring.
 */
UTILITY_API std::wstring& string_to_wstring(
    const std::string& input, std::wstring& output);

/**
 * @brief Convert std::wstring to local 8-bit string or std::string.
 * @param input The input std::wstring to convert.
 * @param output The converted output string.
 * @return Reference to the converted string.
 */
UTILITY_API std::string& wstring_to_string(
    const std::wstring& input, std::string& output);

/**
 * @brief Convert UTF-8 string to std::wstring.
 * @param input The UTF-8 input string to convert.
 * @param output The converted output std::wstring.
 * @return Reference to the converted std::wstring.
 */
UTILITY_API std::wstring& utf8_to_wstring(
    const std::string& input, std::wstring& output);

/**
 * @brief Convert std::wstring to UTF-8 string.
 * @param input The input std::wstring to convert.
 * @param output The converted output UTF-8 string.
 * @return Reference to the converted UTF-8 string.
 */
UTILITY_API std::string& wstring_to_utf8(
    const std::wstring& input, std::string& output);

/**
 * @brief Convert UTF-8 string to local 8-bit string or std::string.
 * On Linux, no conversion is done, because UTF-8 is the default.
 * @param input The UTF-8 input string to convert.
 * @param output The converted output string.
 * @return Reference to the converted string.
 */
UTILITY_API std::string& utf8_to_string(
    const std::string& input, std::string& output);

/**
 * @brief Convert local 8-bit string or std::string to UTF-8 string.
 * On Linux, no conversion is done, because UTF-8 is the default.
 * @param input The input string to convert.
 * @param output The converted output UTF-8 string.
 * @return Reference to the converted UTF-8 string.
 */
UTILITY_API std::string& string_to_utf8(
    const std::string& input, std::string& output);

//
// Convenience APIs
//////////////////////////////////////////////////////////////////////////

//! @brief Convert std::wstring to std::string.
UTILITY_API std::string  str(const std::wstring& string); 

//! @brief Convert UTF-8 string to std::string.
UTILITY_API std::string  str_u8(const std::string& string); 

//! @brief Convert std::wstring to UTF-8 string.
UTILITY_API std::string  utf8(const std::wstring& string); 

//! @brief Convert UTF-8 string to UTF-8 string.
UTILITY_API std::string  utf8(const std::string& string); 

//! @brief Convert std::string to std::wstring.
UTILITY_API std::wstring wstr(const std::string& string); 

//! @brief Convert UTF-8 string to std::wstring.
UTILITY_API std::wstring wstr_u8(const std::string& string); 

//! @brief Convert filesystem path to std::string.
UTILITY_API std::string  str(const std::filesystem::path& path); 

//! @brief Convert filesystem path to std::wstring.
UTILITY_API std::wstring wstr(const std::filesystem::path& path); 

//! @brief Convert filesystem path to UTF-8 string.
UTILITY_API std::string  utf8(const std::filesystem::path& path); 

#ifdef UTILITY_SUPPORT_QT

//! @brief Convert QString to std::string.
UTILITY_API std::string  str (const QString& string); 

//! @brief Convert QString to UTF-8 string.
UTILITY_API std::string  utf8(const QString& string); 

//! @brief Convert QString to std::wstring.
UTILITY_API std::wstring wstr(const QString& string); 

//! @brief Convert std::string to QString.
UTILITY_API QString qstr(const std::string& string); 

//! @brief Convert std::wstring to QString.
UTILITY_API QString qstr(const std::wstring& string); 

//! @brief Convert UTF-8 string to QString.
UTILITY_API QString qstr_u8(const std::string& string); 

//! @brief Convert filesystem path to QString.
UTILITY_API QString qstr(const std::filesystem::path& path); 


#endif // UTILITY_SUPPORT_QT

} // namespace UTILITY_NAMESPACE

#endif // string_conv_h__