#ifndef string_conv_h__
#define string_conv_h__

/*
*   string_conv.hpp 
*
*   v0.1  2018-04 By GuoJH
*   v0.2  2020-08 By GuoJH
*/

#include <string>
#include <utility.hpp>

namespace util {
namespace conv {

// ansi string to utf16 string.
UTILITY_FUNCT_DECL std::wstring& ansi2utf16(
    const std::string& src, std::wstring& dest);

// utf16 string to ansi string.
UTILITY_FUNCT_DECL std::string& utf162ansi(
    const std::wstring& src, std::string& dest);

// utf8 string to utf16 string.
UTILITY_FUNCT_DECL std::wstring& utf82utf16(
    const std::string& src, std::wstring& dest);

// utf16 string to utf8 string.
UTILITY_FUNCT_DECL std::string& utf162utf8(
    const std::wstring& str, std::string& dest);

// utf8 string to ansi string.
UTILITY_FUNCT_DECL std::string& utf82ansi(
    const std::string& src, std::string& dest);

// ansi string to utf8 string.
UTILITY_FUNCT_DECL std::string& ansi2utf8(
    const std::string& src, std::string& dest);

// return the persents of replacement character in utf16.
// https://en.wikipedia.org/wiki/Specials_(Unicode_block)
UTILITY_FUNCT_DECL float utf16_replacement_character_persents(
    const std::wstring& utf);

} // conv
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/string_conv.ipp"
#endif

#endif // string_conv_h__
