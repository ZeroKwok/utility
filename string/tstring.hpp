#ifndef tstring_h__
#define tstring_h__

/*
*   tstring.hpp 
*
*   v0.1  2018-04 By GuoJH
*   v0.2  2020-08 By GuoJH
*/

#include <string>
#include <sstream>
#include <utility.hpp>
#include <string/string_conv.hpp>

#ifdef UTILITY_SUPPORT_QT
#   include <QString>
#endif

namespace util {

/*
*	提供一个STL string 的包装
*   1. 该包装允许以下类型构造:
*       const char*, const wchar_t*, std::string, std::wstring, QString
*
*   2. 支持编码集:
*       ansi, utf8, utf16
*
*   3. 内部维护持有一个utf16(std::wstring), 若通过其他字符集构造,
*      则会产生转码, 至于为什么使用std::wstring, 是因为默认将工程视为
*      UNICODE编码, 那么使用最为频繁的应是std::wstring
*/
class UTILITY_CLASS_DECL tstring : public std::wstring
{
    UTILITY_DECL_PRIVATE(tstring);
public:
    typedef std::wstring supper_type;
    typedef wchar_t      element_type;

    enum coded_format
    {
        format_ansi = 1,
        format_utf8 = 2,
    };

    UTILITY_MEMBER_DECL tstring();
    UTILITY_MEMBER_DECL tstring(const char* right, coded_format format = format_ansi);
    UTILITY_MEMBER_DECL tstring(const wchar_t* right);
    UTILITY_MEMBER_DECL tstring(const std::string& right, coded_format format = format_ansi);
    UTILITY_MEMBER_DECL tstring(const std::wstring& right);
    UTILITY_MEMBER_DECL tstring(const tstring& right);
    UTILITY_MEMBER_DECL ~tstring();

#ifdef UTILITY_SUPPORT_QT
    UTILITY_MEMBER_DECL tstring(const QString& right);
#endif

    UTILITY_MEMBER_DECL tstring& operator=(const tstring& right);
    UTILITY_MEMBER_DECL operator std::string() const;
    UTILITY_MEMBER_DECL operator std::wstring() const;

#ifdef UTILITY_SUPPORT_QT
    UTILITY_MEMBER_DECL operator QString() const;
#endif

    UTILITY_MEMBER_DECL std::string utf8() const;
    UTILITY_MEMBER_DECL std::string string() const;
    UTILITY_MEMBER_DECL std::wstring wstring() const;

    template<class _Type>
    UTILITY_MEMBER_DECL tstring& operator%(const _Type& arg);
    UTILITY_MEMBER_DECL tstring& operator%(const std::string& arg);
    UTILITY_MEMBER_DECL tstring& operator%(const std::wstring& arg);
};

template<class _Type>
tstring& util::tstring::operator%(const _Type& arg)
{
    return (*this) % (std::wstringstream() << arg).str();
}

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/tstring.ipp"
#endif

#endif // tstring_h__
