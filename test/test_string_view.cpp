#include "utility/string.h"
#include "gtest/gtest.h"

using namespace util;

TEST(ToWStringTest, FromWString) {
    std::wstring input = L"Hello Wide";
    auto result = to_wstring(std::wstring_view(input));
    EXPECT_EQ(result, L"Hello Wide");
}

TEST(ToWStringTest, FromString) {
    std::string input = "Hello Narrow";
    auto result = to_wstring(std::string_view(input));
    EXPECT_EQ(result, L"Hello Narrow"); // wstr 模拟为简单拷贝
}

#if __cpp_char8_t
TEST(ToWStringTest, FromUTF8String) {
    std::u8string input = u8"Hello UTF8";
    auto result = to_wstring(std::u8string_view(input));
    EXPECT_EQ(result, L"Hello UTF8");
}
#endif

TEST(ToWStringTest, FromConstCharPointer) {
    const char* cstr = "Hello CStr";
    auto result = to_wstring(std::string_view(cstr));
    EXPECT_EQ(result, L"Hello CStr");
}

TEST(ToWStringTest, FromConstWCharPointer) {
    const wchar_t* wcstr = L"Hello WStr";
    auto result = to_wstring(std::wstring_view(wcstr));
    EXPECT_EQ(result, L"Hello WStr");
}

TEST(ToWStringTest, UsingTStringViewAlias) {
    tstring_view<char> sview("Generic Test");
    auto result = to_wstring(sview);
    EXPECT_EQ(result, L"Generic Test");

    tstring_view<wchar_t> wsview(L"Generic WTest");
    auto result2 = to_wstring(wsview);
    EXPECT_EQ(result2, L"Generic WTest");
}

// ---- registry_set_string 泛型推导测试 ----

template <typename PathT, typename NameT, typename ValueT>
std::tuple<std::wstring, std::wstring, std::wstring>
test_registry_set_string(PathT path, NameT name, ValueT value) {
    std::wstring wpath = to_wstring(make_tstring_view(path));
    std::wstring wname = to_wstring(make_tstring_view(name));
    std::wstring wvalue = to_wstring(make_tstring_view(value));
    return { wpath, wname, wvalue };
}

TEST(ToWStringTest, RegistrySetStringGeneric) {
    {
        auto [wpath, wname, wvalue] = test_registry_set_string("PathA", L"NameB", std::string("ValueC"));
        EXPECT_EQ(wpath, L"PathA");
        EXPECT_EQ(wname, L"NameB");
        EXPECT_EQ(wvalue, L"ValueC");
    }

    {
        auto [wpath, wname, wvalue] = test_registry_set_string(std::string_view("PathA"), std::wstring(L"NameB"), std::wstring_view(L"ValueC"));
        EXPECT_EQ(wpath, L"PathA");
        EXPECT_EQ(wname, L"NameB");
        EXPECT_EQ(wvalue, L"ValueC");
    }

    {
        char path1[] = "PathA";
        wchar_t path2[] = L"NameB";
        auto [wpath, wname, wvalue] = test_registry_set_string(path1, path2, L"ValueC");
        EXPECT_EQ(wpath, L"PathA");
        EXPECT_EQ(wname, L"NameB");
        EXPECT_EQ(wvalue, L"ValueC");
    }
}
