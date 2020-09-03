#ifdef UTILITY_DISABLE_HEADONLY
#   include "../tstring.hpp"
#endif

#include <string/string_util.hpp>
#include <string/string_conv.hpp>
#include <string/string_conv_easy.hpp>

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
    : supper_type(conv::easy::_2wstr(right ? right : "", conv::easy::coded_format(format)))
{
    UTILITY_INIT_PRIVATE(tstring);
}

tstring::tstring(const wchar_t* right)
    : supper_type(right ? right : L"")
{
    UTILITY_INIT_PRIVATE(tstring);
}

tstring::tstring(const std::string& right, coded_format format/* = ansi*/)
    : supper_type(conv::easy::_2wstr(right, conv::easy::coded_format(format)))
{
    UTILITY_INIT_PRIVATE(tstring);
}

tstring::tstring(const std::wstring& right)
    : supper_type(right)
{
    UTILITY_INIT_PRIVATE(tstring);
}

#ifdef UTILITY_SUPPORT_QT
tstring::tstring(const QString& right)
    : supper_type(conv::easy::_2wstr(right))
{
    UTILITY_INIT_PRIVATE(tstring);
}

tstring::operator QString() const
{
    return conv::easy::_2qstr(*this);
}
#endif // UTILITY_SUPPORT_QT

tstring::tstring(const tstring& right)
    : supper_type(right.wstring())
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
    supper_type::operator=(right.wstring());
    return *this;
}

std::string tstring::utf8() const
{
    return conv::easy::_2utf8(*this);
}

std::string tstring::string() const
{
    return conv::easy::_2str(*this);
}

std::wstring tstring::wstring() const
{
    return conv::easy::_2wstr(*this);
}

util::tstring& tstring::operator%(const char* arg)
{
    return (*this) % conv::easy::_2wstr(arg);
}

util::tstring& tstring::operator%(const wchar_t* arg)
{
    return (*this) % conv::easy::_2wstr(arg);
}

util::tstring& tstring::operator%(const std::string& arg)
{
    return (*this) % conv::easy::_2wstr(arg);
}

util::tstring& tstring::operator%(const std::wstring& arg)
{
    std::wstring specifier =
        util::sformat(L"{%d}", ++UTILITY_PRIVATE(tstring).index);
    util::replace(*this, specifier, arg);

    return *this;
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