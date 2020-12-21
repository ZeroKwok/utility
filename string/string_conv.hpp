#ifndef string_conv_h__
#define string_conv_h__

/*
*   string_conv.hpp 
*
*   v0.1  2018-04 By GuoJH
*   v0.2  2020-08 By GuoJH
*/

#include <string>
#include <string/string_cfg.hpp>

namespace util {
namespace conv {

//! local 8 bit string or std::string to std::wstring.
UTILITY_FUNCT_DECL std::wstring& string_to_wstring(
    const std::string& input, std::wstring& output);

//! std::wstring to local 8 bit string or std::string.
UTILITY_FUNCT_DECL std::string& wstring_to_string(
    const std::wstring& input, std::string& output);

//! utf8 string to std::wstring.
UTILITY_FUNCT_DECL std::wstring& utf8_to_wstring(
    const std::string& input, std::wstring& output);

//! std::wstring to utf8 string.
UTILITY_FUNCT_DECL std::string& wstring_to_utf8(
    const std::wstring& input, std::string& output);

//! utf8 string to local 8 bit string or std::string.
//! On Linux, no conversion is done, because UTF-8 is the default.
UTILITY_FUNCT_DECL std::string& utf8_to_string(
    const std::string& input, std::string& output);

//! local 8 bit string or std::string to utf8 string.
//! On Linux, no conversion is done, because UTF-8 is the default.
UTILITY_FUNCT_DECL std::string& string_to_utf8(
    const std::string& input, std::string& output);

#if OS_WIN

//! return the persents of replacement character in utf16.
//! https://en.wikipedia.org/wiki/Specials_(Unicode_block)
//! 
UTILITY_FUNCT_DECL float wstring_replacement_character_persents(
    const std::wstring& utf16);

#endif

} // conv
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/string_conv.ipp"
#endif

#endif // string_conv_h__
