#ifndef string_conv_easy_h__
#define string_conv_easy_h__

/*
*   string_conv_easy.hpp 
*   
*   v0.1  2018-04 By GuoJH
*   v0.2  2020-08 By GuoJH
*/

#ifdef UTILITY_SUPPORT_QT
#   include <QString>
#endif

#ifdef UTILITY_SUPPORT_BSTR
#   include <comutil.h>
#   pragma comment(lib, "comsupp.lib")
#endif

#include <string/string_cfg.hpp>
#include <string/string_conv.hpp>

namespace util {
namespace conv {
namespace easy {

enum coded_format
{
    format_local = 1,   //!< 本地8位编码格式, 根据平台以及地区不同而不同,
                        //!< 例如: Windows 中国大陆: GB2312, Linux: UTF-8;
    format_utf8  = 2,   //!< UTF-8编码格式
};

inline std::string _2str(const std::wstring& string)
{
    std::string result;

    return util::conv::wstring_to_string(string, result);
}

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

inline std::wstring _2wstr(
    const std::string& string, coded_format format = format_local)
{
    std::wstring result;

    if (format == format_utf8)
        return util::conv::utf8_to_wstring(string, result);

    return util::conv::string_to_wstring(string, result);
}

inline std::wstring _2wstr(const std::wstring& string)
{
    return string;
}

inline std::string _2utf8(const std::wstring& string)
{
    std::string result;

    return util::conv::wstring_to_utf8(string, result);
}

inline std::string _2utf8(
    const std::string& string, coded_format format = format_local)
{
    if (format == format_utf8)
        return string;

    std::string result;

    return util::conv::string_to_utf8(string, result);
}

#ifdef UTILITY_SUPPORT_QT

inline std::string _2str(const QString& string)
{
    return string.toStdString();
}

inline std::wstring _2wstr(const QString& string)
{
#if OS_WIN
    return std::wstring((const wchar_t*)string.utf16(), string.length());
#else

    return string.toStdWString();
#endif
}

inline std::string _2utf8(const QString& string)
{
    return util::conv::wstring_to_string(_2wstr(string), std::string());
}

inline QString _2qstr(const std::wstring& string)
{
#if OS_WIN
    /*
    *  在Qt lib中未将wchar_t视为内置类型, 而是作为了unsigned short
    *  所以在使用包含wchar_t的接口时将出现error 2019, 通常, Qt默认
    *  将工程配置修改为不将wchar_t视为内置类型, 但是在使用第三方库时
    *  (默认工程配置, 且带wchar_t的接口)也会出现error 2019错误.
    *
    *  一个折中的办法是, 修改工程设置将wchar_t作为内置类型, 且避免
    *  使用Qt中带有wchar_t的接口. (幸运的是Qt中几乎所有的宽字符接口
    *  都显示声明为 unsigned short*)
    *  _2018-5-11 By GuoJH
    */

    return QString::fromUtf16((const ushort *)string.c_str(), string.size());
#else

    return QString::fromStdWString(string);
#endif
}

inline QString _2qstr(
    const std::string& string, coded_format format = format_local)
{
    if (format == format_utf8)
        return QString::fromUtf8(string.c_str(), string.size());

    return QString::fromStdString(string);
}

#endif

#ifdef UTILITY_SUPPORT_BSTR

inline std::wstring _2wstr(const BSTR& string)
{
    return std::wstring((LPCWSTR)string);
}

inline std::string _2str(const BSTR& string)
{
    return _2str(_2wstr(string));
}

inline std::string _2utf8(const BSTR& string)
{
    return _2utf8(_2wstr(string));
}

inline BSTR _2bstr(const std::wstring& string)
{
    return (BSTR)_bstr_t(string.c_str());
}

inline BSTR _2bstr(const std::string& string, coded_format format = format_local)
{
    return _2bstr(_2wstr(string, format));
}

#endif

} // easy
} // conv
} // util

#endif // string_conv_easy_h__
