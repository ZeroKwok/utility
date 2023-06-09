#include <assert.h>
#include "string_conv_iconv.hpp"

namespace util {
namespace conv {

std::wstring& utf8_to_wstring(
    const std::string& input, std::wstring& output)
{
    return convert_with_iconv(input, output, "UTF-8", "WCHAR_T", false);
}

std::wstring& string_to_wstring(
    const std::string& input, std::wstring& output)
{
    return utf8_to_wstring(input, output);
}

std::string& wstring_to_utf8(
    const std::wstring& input, std::string& output)
{
    return convert_with_iconv(input, output, "WCHAR_T", "UTF-8", false);
}

std::string& wstring_to_string(
    const std::wstring& input, std::string& output)
{
    return wstring_to_utf8(input, output);
}

std::string& utf8_to_string(
    const std::string& input, std::string& output)
{
    return output = input;
}

std::string& string_to_utf8(
    const std::string& input, std::string& output)
{
    return output = input;
}

} // conv
} // util