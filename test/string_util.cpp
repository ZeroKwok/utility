#include <gtest/gtest.h>
#include <string/string_util.hpp>
#include <string/string_conv_easy.hpp>

TEST(string_util, left)
{
    EXPECT_EQ("string", util::left("string/string_conv_easy.hpp", "/"));
    EXPECT_EQ(L"string", util::left(L"string/string_conv_easy.hpp", L"/"));

    EXPECT_EQ(std::string("string"), 
        util::left(std::string("string/string_conv_easy.hpp"), std::string("/")));
    EXPECT_EQ(std::wstring(L"string"),
        util::left(std::wstring(L"string/string_conv_easy.hpp"), std::wstring(L"/")));

    EXPECT_EQ(std::wstring(L""),
        util::left(std::wstring(L"/string_conv_easy.hpp"), std::wstring(L"/")));

    EXPECT_EQ("", util::left("", "/"));
}


TEST(string_util, right)
{
    EXPECT_EQ("string_conv_easy.hpp", util::right("string/string_conv_easy.hpp", "/"));
    EXPECT_EQ(L"string_conv_easy.hpp", util::right(L"string/string_conv_easy.hpp", L"/"));

    EXPECT_EQ(std::string("string_conv_easy.hpp"),
        util::right(std::string("string/string_conv_easy.hpp"), std::string("/")));
    EXPECT_EQ(std::wstring(L"string_conv_easy.hpp"),
        util::right(std::wstring(L"string/string_conv_easy.hpp"), std::wstring(L"/")));

    EXPECT_EQ(std::wstring(L""),
        util::right(std::wstring(L"string/"), std::wstring(L"/")));

    EXPECT_EQ("", util::right("", "/"));
}

TEST(string_util, between)
{
    EXPECT_EQ("_conv_", 
        util::between("string/string_conv_easy.hpp", "string", "easy.hpp"));
    EXPECT_EQ("string_conv_easy.hpp",
        util::between("string/string_conv_easy.hpp", "string", "easy.hpp", util::contains_mark));

    EXPECT_EQ(std::string("_conv_"),
        util::between("string/string_conv_easy.hpp", std::string("string"), "easy.hpp"));
    EXPECT_EQ(L"string_conv_easy.hpp",
        util::between(std::wstring(L"string/string_conv_easy.hpp"), L"string", L"easy.hpp", util::contains_mark));

    EXPECT_EQ("",
        util::between("string/string_conv_easy.hpp", "1", "easy.hpp"));
    EXPECT_EQ("",
        util::between("string/string_conv_easy.hpp", "/", "-"));
}