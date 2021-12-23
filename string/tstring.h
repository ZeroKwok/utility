#ifndef tstring_h__
#define tstring_h__

/*
*   tstring.h
*
*   v0.1  2018-04 By GuoJH
*   v0.2  2020-08 By GuoJH
*/

#include <string>
#include <sstream>
#include <string/string_cfg.h>
#include <string/string_conv.h>

#ifdef UTILITY_SUPPORT_QT
#   include <string/string_conv_easy.hpp>
#   include <QString>
#endif

#ifdef UTILITY_SUPPORT_BOOST
#   include <boost/filesystem/path.hpp>
#endif

#if (_MSC_VER >= 1914 && _MSVC_LANG >= 201703L) || (__GNUC__ >= 8 && __cplusplus >= 201703L)
#   include <filesystem>
#endif

namespace util {

/*!
 *  \brief 提供一个STL string 的包装
 * 
 *  1. 该包装允许以下类型构造:
 *      const char*, const wchar_t*, std::string, std::wstring, QString, boost::filesystem::path.
 *
 *  2. 支持编码集:
 *      Windows ANSI, UTF-8, UTF-16
 *
 *  3. 内部维护Unicode编码字符串, 在Windows上时一个UTF-16(std::wstring)字符串, unix则为UTF-8(std::string).
 * 
 */

#if OS_POSIX
    class UTILITY_CLASS_DECL tstring : public std::string
#else
    class UTILITY_CLASS_DECL tstring : public std::wstring
#endif
{
    UTILITY_DECL_PRIVATE(tstring);
public:

#if OS_POSIX
    typedef std::string         supper_type;
    typedef char                element_type;
    typedef std::stringstream   stream_type;
#else
    typedef std::wstring        supper_type;
    typedef wchar_t             element_type;
    typedef std::wstringstream  stream_type;
#endif

    UTILITY_MEMBER_DECL tstring();
    UTILITY_MEMBER_DECL tstring(const char* right, coded_format format = format_local);
    UTILITY_MEMBER_DECL tstring(const wchar_t* right);
    UTILITY_MEMBER_DECL tstring(const std::string& right, coded_format format = format_local);
    UTILITY_MEMBER_DECL tstring(const std::wstring& right);
    UTILITY_MEMBER_DECL tstring(const tstring& right);
    UTILITY_MEMBER_DECL ~tstring();

#ifdef UTILITY_SUPPORT_QT
    inline tstring(const QString& right);
    inline operator QString() const;
#endif

#ifdef UTILITY_SUPPORT_BOOST
    UTILITY_MEMBER_DECL tstring(const boost::filesystem::path& right);
    UTILITY_MEMBER_DECL operator boost::filesystem::path() const;
#endif

    //! std filesystem
    //! https://zh.cppreference.com/w/cpp/compiler_support
#if (_MSC_VER >= 1914 && _MSVC_LANG >= 201703L) || (__GNUC__ >= 8 && __cplusplus >= 201703L)
    UTILITY_MEMBER_DECL tstring(const std::filesystem::path& right);
    UTILITY_MEMBER_DECL operator std::filesystem::path() const;
    UTILITY_MEMBER_DECL std::filesystem::path std() const;
#endif

    UTILITY_MEMBER_DECL tstring& operator=(const tstring& right);
    UTILITY_MEMBER_DECL operator std::string() const;
    UTILITY_MEMBER_DECL operator std::wstring() const;

    UTILITY_MEMBER_DECL std::string utf8() const;
    UTILITY_MEMBER_DECL std::string string() const;
    UTILITY_MEMBER_DECL std::wstring wstring() const;

    //! 以指定的内容替换字符串中的占位符, 如: {1},{2}
    template<class _Type>
    UTILITY_MEMBER_DECL tstring& operator%(const _Type& arg);
    UTILITY_MEMBER_DECL tstring& operator%(const char* arg);
    UTILITY_MEMBER_DECL tstring& operator%(const wchar_t* arg);
    UTILITY_MEMBER_DECL tstring& operator%(const std::string& arg);
    UTILITY_MEMBER_DECL tstring& operator%(const std::wstring& arg);

    //! 返回指定字符串的索引, 若找不到则返回npos.
    //! 类似于std::string::find()方法
    UTILITY_MEMBER_DECL tstring::size_type index_of(const tstring& arg, tstring::size_type off = 0) const;

private:
    int m_index;
};

template<class _Type>
tstring& util::tstring::operator%(const _Type& arg)
{
    stream_type stream;
    stream << arg;
    return (*this) % stream.str();
}

//!
//! 为了避免库产生Qt Core 的依赖, 这在编写非Qt应用程序时非常重要
//! 
#ifdef UTILITY_SUPPORT_QT
inline tstring::tstring(const QString& right)
#if OS_WIN
    : supper_type(conv::easy::_2wstr(right))
#else
    : supper_type(conv::easy::_2str(right))
#endif
    , m_index(0)
{}

inline tstring::operator QString() const
{
    return conv::easy::_2qstr(*this);
}
#endif // UTILITY_SUPPORT_QT

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/tstring.ipp"
#endif

#endif // tstring_h__
