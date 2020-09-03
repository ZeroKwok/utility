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
    format_ansi = 1,
    format_utf8 = 2,
};

inline std::string _2str(const std::wstring& string)
{
    return util::conv::utf162ansi(string, std::string());
}

inline std::string _2str(
    const std::string& string, coded_format format = format_ansi)
{
    if (format == format_utf8)
        return util::conv::utf82ansi(string, std::string());

    return string;
}

inline std::wstring _2wstr(
    const std::string& string, coded_format format = format_ansi)
{
    if (format == format_utf8)
        return util::conv::utf82utf16(string, std::wstring());

    return util::conv::ansi2utf16(string, std::wstring());
}

inline std::wstring _2wstr(const std::wstring& string)
{
    return string;
}

inline std::string _2utf8(const std::wstring& string)
{
    return util::conv::utf162utf8(string, std::string());
}

inline std::string _2utf8(
    const std::string& string, coded_format format = format_ansi)
{
    if (format == format_utf8)
        return string;

    return util::conv::ansi2utf8(string, std::string());
}

#ifdef UTILITY_SUPPORT_QT

inline std::string _2str(const QString& string)
{
    return string.toStdString();
}

inline std::wstring _2wstr(const QString& string)
{
    return std::wstring((const wchar_t*)string.utf16(), string.length());
}

inline std::string _2utf8(const QString& string)
{
    return util::conv::utf162ansi(_2wstr(string), std::string());
}

inline QString _2qstr(const std::wstring& string)
{
    /*
    *  ��Qt lib��δ��wchar_t��Ϊ��������, ������Ϊ��unsigned short
    *  ������ʹ�ð���wchar_t�Ľӿ�ʱ������error _2019, ͨ��, QtĬ��
    *  �����������޸�Ϊ����wchar_t��Ϊ��������, ������ʹ�õ�������ʱ
    *  (Ĭ�Ϲ�������, �Ҵ�wchar_t�Ľӿ�)Ҳ�����error _2019����.
    *
    *  һ�����еİ취��, �޸Ĺ������ý�wchar_t��Ϊ��������, �ұ���
    *  ʹ��Qt�д���wchar_t�Ľӿ�. (���˵���Qt�м������еĿ��ַ��ӿ�
    *  ����ʾ����Ϊ unsigned short*)
    *  _2018-5-11 By GuoJH
    */

    return QString::fromUtf16((const ushort *)string.c_str(), string.size());
}

inline QString _2qstr(
    const std::string& string, coded_format format = format_ansi)
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

inline BSTR _2bstr(const std::string& string, coded_format format = format_ansi)
{
    return _2bstr(_2wstr(string, format));
}

#endif

} // easy
} // conv
} // util

#endif // string_conv_easy_h__
