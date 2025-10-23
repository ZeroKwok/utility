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

template <typename StringView>
std::wstring to_wstring(StringView str) {
    using CharT = typename StringView::value_type;

    if constexpr (std::is_same_v<CharT, wchar_t>)
        return std::wstring(str);
    else if constexpr (std::is_same_v<CharT, char>)
        return wstr(std::string(str.begin(), str.end()));
#if __cpp_char8_t
    else if constexpr (std::is_same_v<CharT, char8_t>)
        return wstr_u8(std::string(reinterpret_cast<const char*>(str.data()), str.size()));
#endif
    else
        static_assert(sizeof(CharT) == 0, "Unsupported character type");
}

} // namespace UTILITY_NAMESPACE

#endif // string_tstring_view_h__