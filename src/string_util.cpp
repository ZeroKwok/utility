// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#include "utility/string_util.h"

#include <stdarg.h>
#include <string>
#include <algorithm>

namespace UTILITY_NAMESPACE {
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
    typename _TString::size_type beforeLen = before.length();
    typename _TString::size_type afterLen = after.length();
    typename _TString::size_type pos = target.find(before, 0);

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
    typename _TString::size_type index;
    if ((index = target.find(mark)) != _TString::npos)
    {
        return target.substr(0, index);
    }

    return _TString();
}

template<class _TString>
inline _TString _right(const _TString& target, const _TString& mark)
{
    typename _TString::size_type index;
    if ((index = target.rfind(mark)) != _TString::npos)
    {
        return target.substr(index + mark.length());
    }

    return _TString();
}

template<class _TString>
inline std::pair<_TString, _TString> _right_in_half(
    const _TString& target, const _TString& mark)
{
    auto index = target.rfind(mark);
    if (index != _TString::npos)
    {
        return std::make_pair(
            target.substr(0, index),
            target.substr(index + mark.length()));
    }
    return std::pair<_TString, _TString>();
}

template<class _TString>
inline std::pair<_TString, _TString> _left_in_half(
    const _TString& target, const _TString& mark)
{
    auto index = target.find(mark);
    if (index != _TString::npos)
    {
        return std::make_pair(
            target.substr(0, index),
            target.substr(index + mark.length()));
    }
    return std::pair<_TString, _TString>();
}

template<class _TString>
inline _TString _between(
    const _TString& target,
    const _TString& left,
    const _TString& right,
    between_policy policy)
{
    typename _TString::size_type begin, end, middle;
    if ((begin = target.find(left)) != _TString::npos)
    {
        if ((end = target.find(right, begin + 1)) != _TString::npos)
        {
            if (left != right)
            {
                // 寻找最小匹配
                // 例如: a...a...b, a~b
                middle = begin + 1;
                while (middle < end && (middle = target.find(left, middle)) != _TString::npos)
                {
                    if (middle < end)
                        begin = middle++;
                }
            }

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
#if OS_WIN
    va_list list;
    va_start(list, format);

    std::string text(_vscprintf(format, list) + 1, 0);
    vsprintf_s(&text[0], text.size(), format, list);

    va_end(list);

    return text.data();

#else
    va_list list;
    va_start(list, format);

    std::string text(vsnprintf(nullptr, 0, format, list) + 1, 0);

    // These functions do not call the va_end macro. 
    // Because they invoke the va_arg macro, the value of ap is 
    // undefined after the call.
    // https://linux.die.net/man/3/vsnprintf
    va_start(list, format);

    size_t size = vsnprintf(&text[0], text.size(), format, list);

    return text.data();
#endif
}

std::wstring sformat(const wchar_t * format, ...)
{
#if OS_WIN
    va_list list;
    va_start(list, format);

    std::wstring text(_vscwprintf(format, list) + 1, 0);
    vswprintf_s(&text[0], text.size(), format, list);

    va_end(list);

    return text.data();

#else

    va_list list;
    va_start(list, format);

    // 窄字符串提供 std::vsnprintf ，它使得程序能够确定要求的输出缓冲区大小。
    // 不过宽字符串无等价版本，而且为确定缓冲区大小，程序需要调用 std::vswprintf 并检查结果值，
    // 再重新分配更大的缓冲区，反复尝试直至成功。
    // https://zh.cppreference.com/w/cpp/io/c/vfwprintf
    
    int size = 0;
    std::wstring text(1024, 0);

    while ((size = vswprintf(&text[0], text.size(), format, list)) < 0)
    {
        if (text.size() > 0x500000) // 上限5MB
            throw std::runtime_error("The formatted content is too long.");

        text.resize(text.size() + 1024);

        va_start(list, format);
    }

    va_end(list);

    text.resize(size);

    return text;
#endif
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
    std::string result(target);

    return detail::_replace(result, before, after);
}

std::wstring replace_copy(
    const std::wstring& target,
    const std::wstring& before,
    const std::wstring& after)
{
    std::wstring result(target);

    return detail::_replace(result, before, after);
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

std::pair<std::string, std::string> in_half_from_left(
    const std::string& target, const std::string& mark)
{
    return detail::_left_in_half(target, mark);
}

std::pair<std::wstring, std::wstring> in_half_from_left(
    const std::wstring& target, const std::wstring& mark)
{
    return detail::_left_in_half(target, mark);
}

std::pair<std::string, std::string> in_half_from_right(
    const std::string& target, const std::string& mark)
{
    return detail::_right_in_half(target, mark);
}

std::pair<std::wstring, std::wstring> in_half_from_right(
    const std::wstring& target, const std::wstring& mark)
{
    return detail::_right_in_half(target, mark);
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

bool start_with(const std::string& target, const std::string& head)
{
    if (head.empty())
        return true;

    if (head.length() > target.length())
        return false;

    return target.compare(0, head.length(), head) == 0;
}

bool start_with(const std::wstring& target, const std::wstring& head)
{
    if (head.empty())
        return true;

    if (head.length() > target.length())
        return false;

    return target.compare(0, head.length(), head) == 0;
}

bool end_with(const std::string& target, const std::string& tail)
{
    if (tail.empty())
        return true;

    if (tail.length() > target.length())
        return false;

    return target.compare(target.size() - tail.length(), tail.length(), tail) == 0;
}

bool end_with(const std::wstring& target, const std::wstring& tail)
{
    if (tail.empty())
        return true;

    if (tail.length() > target.length())
        return false;

    return target.compare(target.size() - tail.length(), tail.length(), tail) == 0;
}

} // namespace UTILITY_NAMESPACE
