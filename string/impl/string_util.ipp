#ifdef UTILITY_DISABLE_HEADONLY
#   include "../string_util.hpp"
#endif

#include <stdarg.h>
#include <string>
#include <algorithm>

namespace util {
namespace detail {

template<class _TString>
inline _TString __to_lower(const _TString& str)
{
    _TString result(str.length(), 0);
    std::transform(str.begin(), str.end(), result.begin(), ::tolower); 
    return result;
}

template<class _TString>
inline _TString __to_upper(const _TString& str)
{
    _TString result(str.length(), 0);
    std::transform(str.begin(), str.end(), result.begin(), ::toupper); 
    return result;
}

template<class _TString>
inline _TString& _replace(
          _TString& target,
    const _TString& before,
    const _TString& after)
{  
    _TString::size_type beforeLen = before.length();  
    _TString::size_type afterLen  = after.length();  
    _TString::size_type pos       = target.find(before, 0);

    while(pos != _TString::npos)  
    {  
        target.replace(pos, beforeLen, after);  
        pos = target.find(before, (pos + afterLen));  
    }

    return target;
}

template<class _TString>
inline _TString _left(const _TString& target, const _TString& mark)
{
    _TString::size_type index;
    if ((index = target.find(mark)) != _TString::npos)
    {
        return target.substr(0, index);
    }

    return _TString();
}

template<class _TString>
inline _TString _right(const _TString& target, const _TString& mark)
{
    _TString::size_type index;
    if ((index = target.rfind(mark)) != _TString::npos)
    {
        return target.substr(index + mark.length());
    }

    return _TString();
}

template<class _TString>
inline _TString _between(
    const _TString& target,
    const _TString& left,
    const _TString& right,
    between_policy policy)
{
    _TString::size_type begin, end, middle;
    if ((begin = target.find(left)) != _TString::npos)
    {
        if ((end = target.find(right, begin)) != _TString::npos)
        {
            // —∞’“◊Ó–°∆•≈‰
            middle = begin + 1;
            while (middle < end && (middle = target.find(left, middle)) != _TString::npos)
                begin = middle++;

            if (policy & contains_mark)
                return target.substr(begin, end + right.length() - begin);
            else
                return target.substr(begin + left.length(), end - begin - left.length());
        }
    }

    return _TString();
}

} //detail

std::string  sformat(const char * format, ...)
{
    va_list marker = NULL;
    va_start(marker, format);

    std::string text(_vscprintf(format, marker) + 1, 0);
    vsprintf_s(&text[0], text.capacity(), format, marker);
    va_end(marker);

    return text.data();
}

std::wstring sformat(const wchar_t * format, ...)
{
    va_list marker = NULL;
    va_start(marker, format);

    std::wstring text(_vscwprintf(format, marker) + 1, 0);
    vswprintf_s(&text[0], text.capacity(), format, marker);
    va_end(marker);

    return text.data();
}

std::string  to_lower(const std::string& str)
{
    return detail::__to_lower(str);
}

std::wstring to_lower(const std::wstring& str)
{
    return detail::__to_lower(str);
}

std::string  to_upper(const std::string& str)
{
    return detail::__to_upper(str);
}

std::wstring to_upper(const std::wstring& str)
{
    return detail::__to_upper(str);
}

std::string&  replace(std::string& target,
    const std::string& before,
    const std::string& after)
{
    return detail::_replace(target, before, after);
}

std::wstring& replace(std::wstring& target,
    const std::wstring& before,
    const std::wstring& after)
{
    return detail::_replace(target, before, after);
}

std::string replace_copy(
    const std::string& target,
    const std::string& before,
    const std::string& after)
{
    return detail::_replace(std::string(target), before, after);
}

std::wstring replace_copy(
    const std::wstring& target,
    const std::wstring& before,
    const std::wstring& after)
{
    return detail::_replace(std::wstring(target), before, after);
}

std::string left(
    const std::string& target,
    const std::string& mark)
{
    return detail::_left(target, mark);
}

std::wstring left(
    const std::wstring& target,
    const std::wstring& mark)
{
    return detail::_left(target, mark);
}

std::string right(
    const std::string& target,
    const std::string& mark)
{
    return detail::_right(target, mark);
}

std::wstring right(
    const std::wstring& target,
    const std::wstring& mark)
{
    return detail::_right(target, mark);
}

std::string between(
    const std::string& target,
    const std::string& left,
    const std::string& right,
    between_policy policy/* = without_mark*/)
{
    return detail::_between(target, left, right, policy);
}

std::wstring between(
    const std::wstring& target,
    const std::wstring& left,
    const std::wstring& right,
    between_policy policy/* = without_mark*/)
{
    return detail::_between(target, left, right, policy);
}

} // util