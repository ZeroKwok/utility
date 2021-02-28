#ifdef UTILITY_DISABLE_HEADONLY
#   include "../tstring.h"
#endif

#include <string/string_util.h>
#include <string/string_conv.h>
#include <string/string_conv_easy.hpp>

using namespace util::conv::easy;

#if OS_POSIX
#   define _2tstr(...) _2str(__VA_ARGS__)
#else
#   define _2tstr(...) _2wstr(__VA_ARGS__) 
#endif

namespace util {
namespace privately {

//  privately
struct tstring_private
{
    int index;

    tstring_private() : index(0)
    {}
};

} // privately

tstring::tstring()
{
    UTILITY_INIT_PRIVATE(tstring);
}

tstring::tstring(const char* right, coded_format format/* = ansi*/)
    : supper_type(_2tstr(right ? right : "", coded_format(format)))
{
    UTILITY_INIT_PRIVATE(tstring);
}

tstring::tstring(const wchar_t* right)
    : supper_type(_2tstr(right ? right : L""))
{
    UTILITY_INIT_PRIVATE(tstring);
}

tstring::tstring(const std::string& right, coded_format format/* = ansi*/)
    : supper_type(_2tstr(right, coded_format(format)))
{
    UTILITY_INIT_PRIVATE(tstring);
}

tstring::tstring(const std::wstring& right)
    : supper_type(_2tstr(right))
{
    UTILITY_INIT_PRIVATE(tstring);
}

#ifdef UTILITY_SUPPORT_QT
tstring::tstring(const QString& right)
    : supper_type(_2tstr(right))
{
    UTILITY_INIT_PRIVATE(tstring);
}

tstring::operator QString() const
{
    return _2qstr(*this);
}
#endif // UTILITY_SUPPORT_QT

#ifdef UTILITY_SUPPORT_BOOST
tstring::tstring(const boost::filesystem::path& right)
#if OS_POSIX
    : supper_type(right.string())
#else
    : supper_type(right.wstring())
#endif 
{
    UTILITY_INIT_PRIVATE(tstring);
}

tstring::operator boost::filesystem::path() const
{
#if OS_POSIX
    return string();
#else
    return wstring();
#endif 
}
#endif // UTILITY_SUPPORT_BOOST

#if (_MSC_VER >= 1914 && _MSVC_LANG >= 201703L) || (__GNUC__ >= 8 && __cplusplus >= 201703L)
tstring::tstring(const std::filesystem::path& right)
#if OS_POSIX
    : supper_type(right.string())
#else
    : supper_type(right.wstring())
#endif 
{
    UTILITY_INIT_PRIVATE(tstring);
}

tstring::operator std::filesystem::path() const
{
#if OS_POSIX
    return string();
#else
    return wstring();
#endif 
}
#endif // std filesystem

tstring::tstring(const tstring& right)
    : supper_type(right)
{
    UTILITY_INIT_PRIVATE(tstring);
    UTILITY_BOTH_PRIVATE_COPY(tstring, (*this), right);
}

tstring::~tstring()
{
    UTILITY_FREE_PRIVATE(tstring);
}

tstring& tstring::operator=(const tstring& right)
{
    UTILITY_BOTH_PRIVATE_COPY(tstring, (*this), right);

#if COMPILER_MSVC
    supper_type::operator=(right.wstring());
#else
    supper_type::operator=(right.string());
#endif

    return *this;
}

std::string tstring::utf8() const
{
    return _2utf8(*this);
}

std::string tstring::string() const
{
#if COMPILER_MSVC
    return _2str(*this);
#else
    return *this;
#endif
}

std::wstring tstring::wstring() const
{
#if COMPILER_MSVC
    return *this;
#else
    return _2wstr(*this);
#endif
}

util::tstring& tstring::operator%(const char* arg)
{
    return (*this) % _2tstr(arg);
}

util::tstring& tstring::operator%(const wchar_t* arg)
{
    return (*this) % _2tstr(arg);
}

util::tstring& tstring::operator%(const std::string& arg)
{
#if COMPILER_MSVC
    return (*this) % _2tstr(arg);
#else

    std::string specifier =
        util::sformat("{%d}", ++UTILITY_PRIVATE(tstring).index);
    util::replace(*this, specifier, arg);

    return *this;
#endif
}

util::tstring& tstring::operator%(const std::wstring& arg)
{
#if COMPILER_MSVC
    std::wstring specifier =
        util::sformat(L"{%d}", ++UTILITY_PRIVATE(tstring).index);
    util::replace(*this, specifier, arg);

    return *this;
#else

    return (*this) % _2tstr(arg);
#endif
}

tstring::size_type tstring::index_of(const tstring& arg, tstring::size_type off/* = 0*/) const
{
    return find(arg, off);
}

tstring::operator std::string() const
{
    return string();
}

tstring::operator std::wstring() const
{
    return wstring();
}

} // util