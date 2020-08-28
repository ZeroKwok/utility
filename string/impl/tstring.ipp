#ifdef UTILITY_DISABLE_HEADONLY
#   include "../tstring.hpp"
#endif

#include <string/string_conv.hpp>
#include <string/string_conv_easy.hpp>

namespace util {

tstring::tstring()
{}

tstring::tstring(const char* right, coded_format format/* = ansi*/)
    : supper_type(conv::easy::_2wstr(right, conv::easy::coded_format(format)))
{}

tstring::tstring(const wchar_t* right)
    : supper_type(right)
{}

tstring::tstring(const std::string& right, coded_format format/* = ansi*/)
    : supper_type(conv::easy::_2wstr(right, conv::easy::coded_format(format)))
{}

tstring::tstring(const std::wstring& right)
    : supper_type(right)
{}

#ifdef UTILITY_SUPPORT_QT
tstring::tstring(const QString& right)
    : supper_type(conv::easy::_2wstr(right))
{}

tstring::operator QString() const
{
    return conv::easy::_2qstr(*this);
}
#endif // UTILITY_SUPPORT_QT

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

util::tstring& tstring::operator%(const std::string& arg)
{
    return (*this) % conv::easy::_2wstr(arg);
}

util::tstring& tstring::operator%(const std::wstring& arg)
{
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