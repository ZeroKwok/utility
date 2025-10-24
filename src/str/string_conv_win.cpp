// This file is part of the utility project.
// Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#include "utility/string.h"

#ifdef OS_WIN

#    include <assert.h>
#    include <windows.h>

namespace UTILITY_NAMESPACE {

inline bool multibyte_to_widechar(
    const std::string_view& input, std::wstring& output, UINT codepage) {
    if (input.empty()) {
        output.clear();
        return true;
    }

    int len = ::MultiByteToWideChar(
        codepage, 0,
        input.data(), input.size(), nullptr, 0);
    if (len <= 0)
        return false;
    output.resize(len);
    ::MultiByteToWideChar(
        codepage, 0,
        input.data(), input.size(),
        output.data(), len);
    return true;
}

inline bool widechar_to_multibyte(
    const std::wstring_view& input, std::string& output, UINT codepage) {
    if (input.empty()) {
        output.clear();
        return true;
    }

    int len = ::WideCharToMultiByte(
        codepage, 0,
        input.data(), input.size(),
        nullptr, 0, nullptr, nullptr);
    if (len <= 0)
        return false;

    output.resize(len);
    ::WideCharToMultiByte(
        codepage, 0,
        input.data(), input.size(),
        output.data(), len, nullptr, nullptr);
    return true;
}

std::wstring& string_to_wstring(
    const std::string_view& input, std::wstring& output) {
    multibyte_to_widechar(input, output, CP_ACP);
    return output;
}

std::string& wstring_to_string(
    const std::wstring_view& input, std::string& output) {
    widechar_to_multibyte(input, output, CP_ACP);
    return output;
}

std::wstring& utf8_to_wstring(
    const std::string_view& input, std::wstring& output) {
    multibyte_to_widechar(input, output, CP_UTF8);
    return output;
}

std::string& wstring_to_utf8(
    const std::wstring_view& input, std::string& output) {
    widechar_to_multibyte(input, output, CP_UTF8);
    return output;
}

std::string& utf8_to_string(
    const std::string_view& input, std::string& output) {
    std::wstring buff;
    multibyte_to_widechar(input, buff, CP_UTF8);
    widechar_to_multibyte(buff, output, CP_ACP);
    return output;
}

std::string& string_to_utf8(
    const std::string_view& input, std::string& output) {
    std::wstring buff;
    multibyte_to_widechar(input, buff, CP_ACP);
    widechar_to_multibyte(buff, output, CP_UTF8);
    return output;
}

} // UTILITY_NAMESPACE

#endif // OS_WIN