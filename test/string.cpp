#include <gtest/gtest.h>
#include <string/tstring.hpp>
#include <string/string_conv_easy.hpp>

TEST(tstring, default_constructor)
{
    util::tstring string;
    EXPECT_EQ("", string.utf8());
    EXPECT_EQ("", string.string());
    EXPECT_EQ(L"", string.wstring());
}

TEST(tstring, char_constructor)
{
    {
        util::tstring string((char*)nullptr);
        EXPECT_EQ("", string.utf8());
        EXPECT_EQ("", string.string());
        EXPECT_EQ(L"", string.wstring());
    }

    {
        util::tstring string("Hello World!");
        EXPECT_EQ("Hello World!", string.utf8());
        EXPECT_EQ("Hello World!", string.string());
        EXPECT_EQ(L"Hello World!", string.wstring());
    }

    {
        util::tstring string("我们的征程是星辰大海~~~");
        EXPECT_EQ(u8"我们的征程是星辰大海~~~", string.utf8());
        EXPECT_EQ("我们的征程是星辰大海~~~", string.string());
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }

    {
        util::tstring string(u8"我们的征程是星辰大海~~~", util::tstring::format_utf8);
        EXPECT_EQ(u8"我们的征程是星辰大海~~~", string.utf8());
        EXPECT_EQ("我们的征程是星辰大海~~~", string.string());
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }
}

TEST(tstring, wchar_t_constructor)
{
    {
        util::tstring string((wchar_t*)nullptr);
        EXPECT_EQ("", string.utf8());
        EXPECT_EQ("", string.string());
        EXPECT_EQ(L"", string.wstring());
    }

    {
        util::tstring string(L"Hello World!");
        EXPECT_EQ("Hello World!", string.utf8());
        EXPECT_EQ("Hello World!", string.string());
        EXPECT_EQ(L"Hello World!", string.wstring());
    }

    {
        util::tstring string(L"我们的征程是星辰大海~~~");
        EXPECT_EQ(u8"我们的征程是星辰大海~~~", string.utf8());
        EXPECT_EQ("我们的征程是星辰大海~~~", string.string());
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }

    {
        util::tstring string(std::string(u8"我们的征程是星辰大海~~~"), util::tstring::format_utf8);
        EXPECT_EQ(u8"我们的征程是星辰大海~~~", string.utf8());
        EXPECT_EQ("我们的征程是星辰大海~~~", string.string());
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }
}

TEST(tstring, string_constructor)
{
    {
        util::tstring string(std::string(""));
        EXPECT_EQ("", string.utf8());
        EXPECT_EQ("", string.string());
        EXPECT_EQ(L"", string.wstring());
    }

    {
        util::tstring string(std::string("Hello World!"));
        EXPECT_EQ("Hello World!", string.utf8());
        EXPECT_EQ("Hello World!", string.string());
        EXPECT_EQ(L"Hello World!", string.wstring());
    }

    {
        util::tstring string(std::string("我们的征程是星辰大海~~~"));
        EXPECT_EQ(u8"我们的征程是星辰大海~~~", string.utf8());
        EXPECT_EQ("我们的征程是星辰大海~~~", string.string());
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }
}

TEST(tstring, wstring_constructor)
{
    {
        util::tstring string(std::wstring(L""));
        EXPECT_EQ("", string.utf8());
        EXPECT_EQ("", string.string());
        EXPECT_EQ(L"", string.wstring());
    }

    {
        util::tstring string(std::wstring(L"Hello World!"));
        EXPECT_EQ("Hello World!", string.utf8());
        EXPECT_EQ("Hello World!", string.string());
        EXPECT_EQ(L"Hello World!", string.wstring());
    }

    {
        util::tstring string(std::wstring(L"我们的征程是星辰大海~~~"));
        EXPECT_EQ(u8"我们的征程是星辰大海~~~", string.utf8());
        EXPECT_EQ("我们的征程是星辰大海~~~", string.string());
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }
}


TEST(tstring, copy_constructor)
{
    {
        util::tstring s1;
        util::tstring string = s1;
        EXPECT_EQ(L"", string.wstring());
    }

    {
        util::tstring s1(u8"我们的征程是星辰大海~~~", util::tstring::format_utf8);
        util::tstring string = s1;
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }

    {
        util::tstring s1(u8"我们的征程是星辰大海~~~", util::tstring::format_utf8);
        util::tstring string = std::move(s1);
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }

    {
        util::tstring s1(u8"我们的征程是星辰大海~~~", util::tstring::format_utf8);
        util::tstring string(s1);
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }

    {
        util::tstring s1(u8"我们的征程是星辰大海~~~", util::tstring::format_utf8);
        util::tstring string(std::move(s1));
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }

    {
        util::tstring string, s1(u8"我们的征程是星辰大海~~~", util::tstring::format_utf8);
        string = s1;
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }

    {
        util::tstring string, s1(u8"我们的征程是星辰大海~~~", util::tstring::format_utf8);
        string = std::move(s1);
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }
}


TEST(tstring, operator_constructor)
{
    {
        util::tstring string(u8"Hello {1}!", util::tstring::format_utf8);
        string % "World";
        EXPECT_EQ(L"Hello World!", string.wstring());
    }

    {
        util::tstring string(u8"Hello {1}!", util::tstring::format_utf8);
        string % L"World";
        EXPECT_EQ(L"Hello World!", string.wstring());
    }

    {
        util::tstring string(u8"Hello {1}!", util::tstring::format_utf8);
        string % std::string("World");
        EXPECT_EQ(L"Hello World!", string.wstring());
    }

    {
        util::tstring string(u8"Hello {1}!", util::tstring::format_utf8);
        string % std::wstring(L"World");
        EXPECT_EQ(L"Hello World!", string.wstring());
    }

    {
        util::tstring string(u8"Hello {1} {2}!", util::tstring::format_utf8);
        string % util::tstring(L"World") % util::tstring(L"你好世界");
        EXPECT_EQ(L"Hello World 你好世界!", string.wstring());
    }

    {
        util::tstring string(u8"Hello {1}!", util::tstring::format_utf8);
        string % 2;
        EXPECT_EQ(L"Hello 2!", string.wstring());
    }

    {
        util::tstring string(u8"Hello {1}!", util::tstring::format_utf8);
        string % 2.1;
        EXPECT_EQ(L"Hello 2.1!", string.wstring());
    }

    {
        util::tstring string(u8"个体和{1} 高于{2}和{3}!", util::tstring::format_utf8);
        string % "互动" % "流程" % L"工具";
        EXPECT_EQ(L"个体和互动 高于流程和工具!", string.wstring());
    }

    {
        util::tstring string(u8"个体和{1} 高于{1}和{2}!", util::tstring::format_utf8);
        string % "互动" % "流程" % L"工具";
        EXPECT_EQ(L"个体和互动 高于互动和流程!", string.wstring());
    }

    {
        util::tstring string(u8"个体和{1} 高于{2}和{1}!", util::tstring::format_utf8);
        string % "互动" % "流程" % L"工具";
        EXPECT_EQ(L"个体和互动 高于流程和互动!", string.wstring());
    }

    {
        util::tstring s1(u8"个体和{1} 高于{2}和{1}!", util::tstring::format_utf8);
        util::tstring string = s1;
        string % "互动" % "流程" % L"工具";
        EXPECT_EQ(L"个体和互动 高于流程和互动!", string.wstring());
    }

    {
        util::tstring string, s1(u8"个体和{1} 高于{2}和{1}!", util::tstring::format_utf8);
        string = s1;
        string % "互动" % "流程" % L"工具";
        EXPECT_EQ(L"个体和互动 高于流程和互动!", string.wstring());
    }
}