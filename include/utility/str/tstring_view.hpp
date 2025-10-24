// This file is part of the utility project.
// Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#ifndef string_tstring_view_h__
#define string_tstring_view_h__

#include "strconv.h"
#include "utility/config.h"

#include <string>
#include <string_view>
#include <type_traits>

namespace UTILITY_NAMESPACE {

template <typename CharT>
using tstring_view = std::basic_string_view<CharT>;

/*
 * 根据传入类型生成合适的 basic_string_view
 * 支持：
 * - std::string / const char*
 * - std::wstring / const wchar_t*
 * - std::u8string / char8_t* (若支持)
 */
template <typename T>
auto make_tstring_view(T&& str) {
    using CharT = std::remove_cv_t<std::remove_pointer_t<std::decay_t<T>>>;
    if constexpr (std::is_same_v<CharT, char>)
        return std::string_view(str);
    else if constexpr (std::is_same_v<CharT, wchar_t>)
        return std::wstring_view(str);
#if __cpp_char8_t
    else if constexpr (std::is_same_v<CharT, char8_t>)
        return std::u8string_view(str);
#endif
    else if constexpr (std::is_same_v<typename std::decay_t<T>::value_type, char>)
        return std::string_view(str);
    else if constexpr (std::is_same_v<typename std::decay_t<T>::value_type, wchar_t>)
        return std::wstring_view(str);
#if __cpp_char8_t
    else if constexpr (std::is_same_v<typename std::decay_t<T>::value_type, char8_t>)
        return std::u8string_view(str);
#endif
    else
        static_assert(sizeof(T) == 0, "Unsupported string type");
}

/*
 * 将各种字符串类型统一转换为 std::wstring
 * 支持：
 * - std::string / std::string_view / const char*
 * - std::wstring / std::wstring_view / const wchar_t*
 * - std::u8string / std::u8string_view / char8_t* (若支持)
 */
template <typename T>
std::wstring to_wstring(const T& str) {
    using U = std::decay_t<T>;

    if constexpr (std::is_same_v<U, std::wstring>)
        return str;
    else if constexpr (std::is_same_v<U, std::wstring_view>)
        return std::wstring(str);
    else if constexpr (std::is_same_v<U, wchar_t*>)
        return std::wstring(str);
    else if constexpr (std::is_same_v<U, const wchar_t*>)
        return std::wstring(str);

    else if constexpr (std::is_same_v<U, std::string>)
        return wstr(str);
    else if constexpr (std::is_same_v<U, std::string_view>)
        return wstr(str);
    else if constexpr (std::is_same_v<U, char*> || std::is_same_v<U, const char*>)
        return wstr(str);

#if __cpp_char8_t
    else if constexpr (std::is_same_v<U, std::u8string> ||      //
                       std::is_same_v<U, std::u8string_view> || //
                       std::is_same_v<U, char8_t*> ||           //
                       std::is_same_v<U, const char8_t*>)
        return wstr_u8(reinterpret_cast<const char*>(std::data(str)));
#endif
    else
        static_assert(sizeof(U) == 0, "Unsupported string type for to_wstring()");
}

} // namespace UTILITY_NAMESPACE

#endif // string_tstring_view_h__