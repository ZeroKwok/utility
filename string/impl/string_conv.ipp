#include <utility.hpp>
#ifdef UTILITY_DISABLE_HEADONLY
#   include "../string_conv.hpp"
#endif

#include <assert.h>
#include <windows.h>

namespace util {
namespace conv {
namespace detail {

inline const uintptr_t _ansi2utf16(const char* psrc, wchar_t* pdest)
{
    assert(psrc);
    int len = ::MultiByteToWideChar(CP_ACP, 0, psrc, -1, NULL, 0);

    if( !pdest )
        return (uintptr_t)len;

    ::MultiByteToWideChar(CP_ACP, 0, psrc, -1, (LPWSTR)pdest, len);
    return (uintptr_t)pdest;
}

inline const uintptr_t _utf162ansi(const wchar_t* psrc, char* pdest)
{
    assert(psrc);
    int len = ::WideCharToMultiByte(CP_ACP, 0, psrc, -1, NULL, 
        0, NULL, NULL);

    if( !pdest )
        return (uintptr_t)len;
    ::WideCharToMultiByte(CP_ACP, 0, psrc, -1, pdest, len, NULL, NULL);
    return (uintptr_t)pdest;
}

inline const uintptr_t _utf82utf16(const char* psrc, wchar_t* pdest)
{
    assert(psrc);
    int len = ::MultiByteToWideChar(CP_UTF8, 0, psrc, -1, NULL, 0);

    if( !pdest )
        return (uintptr_t)len;

    ::MultiByteToWideChar(CP_UTF8, 0, psrc, -1, (LPWSTR)pdest, len);
    return (uintptr_t)pdest;
}

inline const uintptr_t _utf162utf8(const wchar_t* psrc, char* pdest)
{
    assert(psrc);
    int len = ::WideCharToMultiByte(CP_UTF8, 0, psrc, -1, NULL, 
        0, NULL, NULL);

    if( !pdest )
        return (uintptr_t)len;
    ::WideCharToMultiByte(CP_UTF8, 0, psrc, -1, pdest, len, NULL, NULL);
    return (uintptr_t)pdest;
}

} // detail
std::wstring& ansi2utf16(
    const std::string& src, std::wstring& dest)
{
    size_t len = detail::_ansi2utf16(src.c_str(), 0);

    dest.resize(len);
    detail::_ansi2utf16(src.c_str(), const_cast<wchar_t*>(dest.data()));

    if (dest[dest.size() - 1] == '\0')
        dest.erase(dest.begin() + (dest.size() - 1));

    return dest;
}

std::string& utf162ansi(
    const std::wstring& src, std::string& dest)
{
    size_t len = detail::_utf162ansi(src.c_str(), 0);

    dest.resize(len);
    detail::_utf162ansi(src.c_str(), const_cast<char*>(dest.data()));

    if (dest[dest.size() - 1] == '\0')
        dest.erase(dest.begin() + (dest.size() - 1));

    return dest;
}

std::wstring& utf82utf16(
    const std::string& src, std::wstring& dest)
{
    size_t len = detail::_utf82utf16(src.c_str(), 0);

    dest.resize(len);
    detail::_utf82utf16(src.c_str(), const_cast<wchar_t*>(dest.data()));

    if (dest[dest.size() - 1] == '\0')
        dest.erase(dest.begin() + (dest.size() - 1));

    return dest;
}

std::string& utf162utf8(
    const std::wstring& str, std::string& dest)
{
    size_t len = detail::_utf162utf8(str.c_str(), 0);

    dest.resize(len);
    detail::_utf162utf8(str.c_str(), const_cast<char*>(dest.data()));

    if (dest[dest.size() - 1] == '\0')
        dest.erase(dest.begin() + (dest.size() - 1));

    return dest;
}

std::string& utf82ansi(
    const std::string& src, std::string& dest)
{
    std::wstring buff;
    utf82utf16(src, buff);
    utf162ansi(buff, dest);

    return dest;
}

std::string& ansi2utf8(
    const std::string& src, std::string& dest)
{
    std::wstring buff;
    ansi2utf16(src, buff);
    utf162utf8(buff, dest);

    return dest;
}

float utf16_replacement_character_persents(
    const std::wstring& utf)
{
    float count = 0;
    for each(auto ch in utf)
    {
        if (ch == 0xfffd)
            ++count;
    }

    return count / utf.size() * 100;
}

} // conv
} // util