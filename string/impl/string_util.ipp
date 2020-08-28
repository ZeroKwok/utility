#ifdef UTILITY_DISABLE_HEADONLY
#   include "../string_util.hpp"
#endif

#include <algorithm>

namespace util {
namespace detail {

template<class _TChar>
inline std::basic_string<_TChar> __tolower(const std::basic_string<_TChar>& str)
{
    std::basic_string<_TChar> result(str.length(), 0);
    std::transform(str.begin(), str.end(), result.begin(), ::tolower); 
    return result;
}

template<class _TChar>
inline std::basic_string<_TChar> __toupper(const std::basic_string<_TChar>& str)
{
    std::basic_string<_TChar> result(str.length(), 0);
    std::transform(str.begin(), str.end(), result.begin(), ::toupper); 
    return result;
}

template<class _TChar>
inline std::basic_string<_TChar>& _replace(std::basic_string<_TChar>& target, 
                                     const std::basic_string<_TChar>& before, 
                                     const std::basic_string<_TChar>& after)
{  
    std::basic_string<_TChar>::size_type beforeLen = before.length();  
    std::basic_string<_TChar>::size_type afterLen  = after.length();  
    std::basic_string<_TChar>::size_type pos       = target.find(before, 0);

    while(pos != std::basic_string<_TChar>::npos)  
    {  
        target.replace(pos, beforeLen, after);  
        pos = target.find(before, (pos + afterLen));  
    }

    return target;
}

template<class _TChar>
inline std::pair<std::basic_string<_TChar>, std::basic_string<_TChar> > 
    _right_split(const std::basic_string<_TChar>& target, 
                 const std::basic_string<_TChar>& search)
{
    std::basic_string<_TChar>::size_type index;
    if( (index = target.rfind(search)) != std::basic_string<_TChar>::npos ) 
    {
        return std::make_pair(target.substr(0, index), 
                              target.substr(index + search.length()));
    }

    return std::pair<std::basic_string<_TChar>, std::basic_string<_TChar> >();
}

template<class _TChar>
inline std::pair<std::basic_string<_TChar>, std::basic_string<_TChar> > 
    _left_split(const std::basic_string<_TChar>& target, 
                const std::basic_string<_TChar>& search)
{
    std::basic_string<_TChar>::size_type index;
    if( (index = target.find(search)) != std::basic_string<_TChar>::npos ) 
    {
        return std::make_pair(target.substr(0, index), 
                              target.substr(index + search.length()));
    }

    return std::pair<std::basic_string<_TChar>, std::basic_string<_TChar> >();
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

std::string  tolower(const std::string& str)
{
    return detail::__tolower(str);
}

std::wstring tolower(const std::wstring& str)
{
    return detail::__tolower(str);
}

std::string  toupper(const std::string& str)
{
    return detail::__toupper(str);
}

std::wstring toupper(const std::wstring& str)
{
    return detail::__toupper(str);
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

std::string  replace_copy(
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

std::pair<std::string, std::string>
    right_split(const std::string& target, const std::string& search)
{
    return detail::_right_split(target, search);
}

std::pair<std::wstring, std::wstring>
    right_split(const std::wstring& target, const std::wstring& search)
{
    return detail::_right_split(target, search);
}

std::pair<std::string, std::string>
    left_split(const std::string& target, const std::string& search)
{
    return detail::_left_split(target, search);
}

std::pair<std::wstring, std::wstring>
    left_split(const std::wstring& target, const std::wstring& search)
{
    return detail::_left_split(target, search);
}

} // util