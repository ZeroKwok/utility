#include <assert.h>
#include <windows.h>

namespace util {
namespace conv {
namespace detail {

inline const uintptr_t _ansi2utf16(const char* input, wchar_t* output)
{
    assert(input);
    int len = ::MultiByteToWideChar(CP_ACP, 0, input, -1, NULL, 0);

    if( !output )
        return (uintptr_t)len;

    ::MultiByteToWideChar(CP_ACP, 0, input, -1, (LPWSTR)output, len);
    return (uintptr_t)output;
}

inline const uintptr_t _utf162ansi(const wchar_t* input, char* output)
{
    assert(input);
    int len = ::WideCharToMultiByte(CP_ACP, 0, input, -1, NULL, 
        0, NULL, NULL);

    if( !output )
        return (uintptr_t)len;
    ::WideCharToMultiByte(CP_ACP, 0, input, -1, output, len, NULL, NULL);
    return (uintptr_t)output;
}

inline const uintptr_t _utf82utf16(const char* input, wchar_t* output)
{
    assert(input);
    int len = ::MultiByteToWideChar(CP_UTF8, 0, input, -1, NULL, 0);

    if( !output )
        return (uintptr_t)len;

    ::MultiByteToWideChar(CP_UTF8, 0, input, -1, (LPWSTR)output, len);
    return (uintptr_t)output;
}

inline const uintptr_t _utf162utf8(const wchar_t* input, char* output)
{
    assert(input);
    int len = ::WideCharToMultiByte(CP_UTF8, 0, input, -1, NULL, 
        0, NULL, NULL);

    if( !output )
        return (uintptr_t)len;
    ::WideCharToMultiByte(CP_UTF8, 0, input, -1, output, len, NULL, NULL);
    return (uintptr_t)output;
}

} // detail

std::wstring& string_to_wstring(
    const std::string& input, std::wstring& output)
{
    size_t len = detail::_ansi2utf16(input.c_str(), 0);

    output.resize(len);
    detail::_ansi2utf16(input.c_str(), const_cast<wchar_t*>(output.data()));

    if (output[output.size() - 1] == '\0')
        output.erase(output.begin() + (output.size() - 1));

    return output;
}

std::string& wstring_to_string(
    const std::wstring& input, std::string& output)
{
    size_t len = detail::_utf162ansi(input.c_str(), 0);

    output.resize(len);
    detail::_utf162ansi(input.c_str(), const_cast<char*>(output.data()));

    if (output[output.size() - 1] == '\0')
        output.erase(output.begin() + (output.size() - 1));

    return output;
}

std::wstring& utf8_to_wstring(
    const std::string& input, std::wstring& output)
{
    size_t len = detail::_utf82utf16(input.c_str(), 0);

    output.resize(len);
    detail::_utf82utf16(input.c_str(), const_cast<wchar_t*>(output.data()));

    if (output[output.size() - 1] == '\0')
        output.erase(output.begin() + (output.size() - 1));

    return output;
}

std::string& wstring_to_utf8(
    const std::wstring& str, std::string& output)
{
    size_t len = detail::_utf162utf8(str.c_str(), 0);

    output.resize(len);
    detail::_utf162utf8(str.c_str(), const_cast<char*>(output.data()));

    if (output[output.size() - 1] == '\0')
        output.erase(output.begin() + (output.size() - 1));

    return output;
}

std::string& utf8_to_string(
    const std::string& input, std::string& output)
{
    std::wstring buff;
    utf8_to_wstring(input, buff);
    wstring_to_string(buff, output);

    return output;
}

std::string& string_to_utf8(
    const std::string& input, std::string& output)
{
    std::wstring buff;
    string_to_wstring(input, buff);
    wstring_to_utf8(buff, output);

    return output;
}

float wstring_replacement_character_persents(
    const std::wstring& utf16)
{
    float count = 0;
    for (auto ch : utf16)
    {
        if (ch == 0xfffd)
            ++count;
    }

    return count / utf16.size() * 100;
}

} // conv
} // util