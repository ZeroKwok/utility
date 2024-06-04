// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.
//
// v0.1  2018-04
// v0.2  2020-12
// v0.3  2024-06

#include "utility/string_conv.h"

namespace UTILITY_NAMESPACE {

std::string str(const std::wstring& string) {
    std::string result;
    return wstring_to_string(string, result);
}

std::string str_u8(const std::string& string) {
    std::string result;
    return utf8_to_string(string, result);
}

std::string  utf8(const std::wstring& string) {
    std::string result;
    return wstring_to_utf8(string, result);
}

std::string  utf8(const std::string& string) {
    std::string result;
    return string_to_utf8(string, result);
}

std::wstring wstr(const std::string& string) {
    std::wstring result;
    return string_to_wstring(string, result);
}

std::wstring wstr_u8(const std::string& string) {
    std::wstring result;
    return utf8_to_wstring(string, result);
}

#ifdef UTILITY_SUPPORT_QT

std::string  str (const QString& string) {
    return string.toStdString();
}

std::string  utf8(const QString& string) {
    return util::conv::wstring_to_utf8(_2wstr(string), std::string());
}

std::wstring wstr(const QString& string)
{
#if OS_WIN
    return std::wstring((const wchar_t*)string.utf16(), string.length());
#else
    return string.toStdWString();
#endif
}

QString qstr(const std::string& string)
{
    // 在 Qt 5.15 中 QString::fromStdString() 的实现如下,
    // 将多字节字符串当成 UTF-8 处理, 当多字节为本地8位编码时将导致乱码.
    // 
    // inline QString QString::fromStdString(const std::string &s)
    // { return fromUtf8(s.data(), int(s.size())); }
    //
    // 2021-12-10 By GuoJH
    
#ifdef OS_WIN
    return QString::fromStdWString(wstr(string));
#else
    return QString::fromStdString(string);
#endif
}

QString qstr(const std::wstring& string)
{
#if OS_WIN
    // 在 Qt lib 中未将 wchar_t 视为内置类型, 反而作为 unsigned short
    // 因此在使用包含 wchar_t 的接口时将出现 error 2019 链接错误.
    // 通常, Qt默认将工程配置修改为不将 wchar_t 视为内置类型, 
    // 但是在使用第三方库时 (默认工程配置, 且带 wchar_t 的接口) 也会出现 error 2019.
    // 因为两个模块间的接口签名不一致, 链接时自然会找不到.
    //
    // 一个折中的办法是, 修改工程设置将 wchar_t 作为内置类型,
    //  且避免使用 Qt 中带有 wchar_t 的接口. 
    // (Qt 中几乎所有的宽字符接口都显示声明为 unsigned short*)
    //
    // 2018-5-11 By GuoJH

    return QString::fromUtf16((const ushort *)string.c_str(), string.size());
#else
    return QString::fromStdWString(string);
#endif
}

QString qstr_u8(const std::wstring& string) {
    return QString::fromUtf8(string.c_str(), string.size());
}

#endif // UTILITY_SUPPORT_QT

} // namespace UTILITY_NAMESPACE
