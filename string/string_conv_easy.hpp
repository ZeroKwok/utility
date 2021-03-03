#ifndef string_conv_easy_h__
#define string_conv_easy_h__

/*
*   string_conv_easy.hpp 
*   
*   v0.1  2018-04 By GuoJH
*   v0.2  2020-12 By GuoJH
*/

#include <string/string_cfg.h>
#include <string/string_conv.h>

#ifdef UTILITY_SUPPORT_QT
#   include <QString>
#endif

#if defined(UTILITY_SUPPORT_BSTR) && defined(OS_WIN)
#   include <comutil.h>
#   pragma comment(lib, "comsupp.lib")
#endif

namespace util {
namespace conv {
namespace easy {

//! 提供宽字节到多字节转换的便捷API
//! 
//! \return 返回多字节转换的结果，该多字节依赖于所在平台，在Windows中依赖于当前语言环境，Linux中默认为UTF-8。
//! 
inline std::string _2str(const std::wstring& string)
{
    std::string result;

    return util::conv::wstring_to_string(string, result);
}

//! 提供多字节到多字节转换的便捷API
//! 
//! \param string 
//! \param format 表示参数string中的字符编码，默认为format_local表示本地多字节编码。
//! \return 返回多字节转换的结果
//! 
inline std::string _2str(
    const std::string& string, coded_format format = format_local)
{
    if (format == format_utf8)
    {
        std::string result;

        return util::conv::utf8_to_string(string, result);
    }

    return string;
}

//! 提供const char* 版本以解决QString, std::string的冲突.
inline std::string _2str(
    const char* string, coded_format format = format_local)
{
    if (format == format_utf8)
    {
        std::string result;

        return util::conv::utf8_to_string(string, result);
    }

    return string;
}

//! 提供多字节到宽字节转换的便捷API
inline std::wstring _2wstr(
    const std::string& string, coded_format format = format_local)
{
    std::wstring result;

    if (format == format_utf8)
        return util::conv::utf8_to_wstring(string, result);

    return util::conv::string_to_wstring(string, result);
}

//! 提供const char* 版本以解决QString, std::string的冲突.
inline std::wstring _2wstr(
    const char* string, coded_format format = format_local)
{
    std::wstring result;

    if (format == format_utf8)
        return util::conv::utf8_to_wstring(string, result);

    return util::conv::string_to_wstring(string, result);
}

//! 提供宽字节到宽字节转换的便捷API
//! 提供该函数的目的是运用于模板
inline std::wstring _2wstr(const std::wstring& string)
{
    return string;
}

//! 提供宽字节到utf-8转换的便捷API
inline std::string _2utf8(const std::wstring& string)
{
    std::string result;

    return util::conv::wstring_to_utf8(string, result);
}

//! 提供宽字节到utf-8转换的便捷API
inline std::string _2utf8(
    const std::string& string, coded_format format = format_local)
{
    if (format == format_utf8)
        return string;

    std::string result;

    return util::conv::string_to_utf8(string, result);
}

//! 提供const char* 版本以解决QString, std::string的冲突.
inline std::string _2utf8(
    const char* string, coded_format format = format_local)
{
    if (format == format_utf8)
        return string;

    std::string result;

    return util::conv::string_to_utf8(string, result);
}

// QString的相关支持
//
#ifdef UTILITY_SUPPORT_QT

//! 提供QString到多字节转换的便捷API
inline std::string _2str(const QString& string)
{
    return string.toStdString();
}

//! 提供QString到宽字节转换的便捷API
inline std::wstring _2wstr(const QString& string)
{
#if OS_WIN
    return std::wstring((const wchar_t*)string.utf16(), string.length());
#else

    return string.toStdWString();
#endif
}

//! 提供QString到utf-8转换的便捷API
inline std::string _2utf8(const QString& string)
{
    return util::conv::wstring_to_string(_2wstr(string), std::string());
}

//! 提供宽字节到QString转换的便捷API
inline QString _2qstr(const std::wstring& string)
{
#if OS_WIN
    //
    // 在Qt lib中未将wchar_t视为内置类型, 而是作为了unsigned short
    // 所以在使用包含wchar_t的接口时将出现error 2019, 通常, Qt默认
    // 将工程配置修改为不将wchar_t视为内置类型, 但是在使用第三方库时
    // (默认工程配置, 且带wchar_t的接口)也会出现error 2019错误.
    //
    // 一个折中的办法是, 修改工程设置将wchar_t作为内置类型, 且避免
    // 使用Qt中带有wchar_t的接口. (幸运的是Qt中几乎所有的宽字符接口
    // 都显示声明为 unsigned short*)
    // 2018-5-11 By GuoJH
    //

    return QString::fromUtf16((const ushort *)string.c_str(), string.size());
#else

    return QString::fromStdWString(string);
#endif
}

//! 提供多字节到QString转换的便捷API
inline QString _2qstr(
    const std::string& string, coded_format format = format_local)
{
    if (format == format_utf8)
        return QString::fromUtf8(string.c_str(), string.size());

    return QString::fromStdString(string);
}

#endif

// BSTR (Basic string or binary string) 的相关支持
// https://docs.microsoft.com/en-us/previous-versions/windows/desktop/automat/bstr
//
#ifdef UTILITY_SUPPORT_BSTR

//! 提供BSTR到宽字节转换的便捷API
inline std::wstring _2wstr(const BSTR& string)
{
    return std::wstring((LPCWSTR)string);
}

//! 提供BSTR到多字节转换的便捷API
inline std::string _2str(const BSTR& string)
{
    return _2str(_2wstr(string));
}

//! 提供BSTR到utf-8转换的便捷API
inline std::string _2utf8(const BSTR& string)
{
    return _2utf8(_2wstr(string));
}

//! 提供宽字节到BSTR转换的便捷API
inline BSTR _2bstr(const std::wstring& string)
{
    return (BSTR)_bstr_t(string.c_str());
}

//! 提供多字节到BSTR转换的便捷API
inline BSTR _2bstr(const std::string& string, coded_format format = format_local)
{
    return _2bstr(_2wstr(string, format));
}

#endif

} // easy
} // conv
} // util

#endif // string_conv_easy_h__
