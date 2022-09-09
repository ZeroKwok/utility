#include <gtest/gtest.h>
#include <string/tstring.h>
#include <string/string_conv_easy.hpp>

//
// 这里需要注意的是, 源文件为utf-8编码
//
using namespace util;
using namespace conv::easy;


TEST(tstring, default_constructor)
{
    tstring string;
    EXPECT_EQ("", string.utf8());
    EXPECT_EQ("", string.string());
    EXPECT_EQ(L"", string.wstring());
}

TEST(tstring, char_constructor)
{
    {
        tstring string((char*)nullptr);
        EXPECT_EQ("", string.utf8());
        EXPECT_EQ("", string.string());
        EXPECT_EQ(L"", string.wstring());
    }

    {
        tstring string("Hello World!");
        EXPECT_EQ("Hello World!", string.utf8());
        EXPECT_EQ("Hello World!", string.string());
        EXPECT_EQ(L"Hello World!", string.wstring());
    }

    {
        tstring string("我们的征程是星辰大海~~~", format_utf8);
        EXPECT_EQ("我们的征程是星辰大海~~~", string.utf8());
        EXPECT_EQ(_2str("我们的征程是星辰大海~~~", format_utf8), string.string());
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }

    {
        tstring string("我们的征程是星辰大海~~~", format_utf8);
        EXPECT_EQ("我们的征程是星辰大海~~~", string.utf8());
        EXPECT_EQ(_2str("我们的征程是星辰大海~~~", format_utf8), string.string());
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }
}

TEST(tstring, wchar_t_constructor)
{
    using namespace conv::easy;
    {
        tstring string((wchar_t*)nullptr);
        EXPECT_EQ("", string.utf8());
        EXPECT_EQ("", string.string());
        EXPECT_EQ(L"", string.wstring());
    }

    {
        tstring string(L"Hello World!");
        EXPECT_EQ("Hello World!", string.utf8());
        EXPECT_EQ("Hello World!", string.string());
        EXPECT_EQ(L"Hello World!", string.wstring());
    }

    {
        tstring string(L"我们的征程是星辰大海~~~");
        EXPECT_EQ("我们的征程是星辰大海~~~", string.utf8());
        EXPECT_EQ(_2str("我们的征程是星辰大海~~~", format_utf8), string.string());
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }

    {
        tstring string(std::string("我们的征程是星辰大海~~~"), format_utf8);
        EXPECT_EQ("我们的征程是星辰大海~~~", string.utf8());
        EXPECT_EQ(_2str("我们的征程是星辰大海~~~", format_utf8), string.string());
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }
}

TEST(tstring, string_constructor)
{
    using namespace conv::easy;
    {
        tstring string(std::string(""));
        EXPECT_EQ("", string.utf8());
        EXPECT_EQ("", string.string());
        EXPECT_EQ(L"", string.wstring());
    }

    {
        tstring string(std::string("Hello World!"));
        EXPECT_EQ("Hello World!", string.utf8());
        EXPECT_EQ("Hello World!", string.string());
        EXPECT_EQ(L"Hello World!", string.wstring());
    }

    {
        tstring string(std::string("我们的征程是星辰大海~~~"), format_utf8);
        EXPECT_EQ("我们的征程是星辰大海~~~", string.utf8());
        EXPECT_EQ(_2str("我们的征程是星辰大海~~~", format_utf8), string.string());
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }
}

TEST(tstring, wstring_constructor)
{
    using namespace conv::easy;
    {
        tstring string(std::wstring(L""));
        EXPECT_EQ("", string.utf8());
        EXPECT_EQ("", string.string());
        EXPECT_EQ(L"", string.wstring());
    }

    {
        tstring string(std::wstring(L"Hello World!"));
        EXPECT_EQ("Hello World!", string.utf8());
        EXPECT_EQ("Hello World!", string.string());
        EXPECT_EQ(L"Hello World!", string.wstring());
    }

    {
        tstring string(std::wstring(L"我们的征程是星辰大海~~~"));
        EXPECT_EQ("我们的征程是星辰大海~~~", string.utf8());
        EXPECT_EQ(_2str("我们的征程是星辰大海~~~", format_utf8), string.string());
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }
}

TEST(tstring, copy_constructor)
{
    using namespace conv::easy;
    {
        tstring s1;
        tstring string = s1;
        EXPECT_EQ(L"", string.wstring());
    }

    {
        tstring s1("我们的征程是星辰大海~~~", format_utf8);
        tstring string = s1;
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }

    {
        tstring s1("我们的征程是星辰大海~~~", format_utf8);
        tstring string = std::move(s1);
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }

    {
        tstring s1("我们的征程是星辰大海~~~", format_utf8);
        tstring string(s1);
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }

    {
        tstring s1("我们的征程是星辰大海~~~", format_utf8);
        tstring string(std::move(s1));
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }

    {
        tstring string, s1("我们的征程是星辰大海~~~", format_utf8);
        string = s1;
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }

    {
        tstring string, s1("我们的征程是星辰大海~~~", format_utf8);
        string = std::move(s1);
        EXPECT_EQ(L"我们的征程是星辰大海~~~", string.wstring());
    }
}

TEST(tstring, operator_constructor)
{
    using namespace conv::easy;
    {
        tstring string("Hello {1}!", format_utf8);
        string % "World";
        EXPECT_EQ(L"Hello World!", string.wstring());
    }

    {
        tstring string("Hello {1}!", format_utf8);
        string % L"World";
        EXPECT_EQ(L"Hello World!", string.wstring());
    }

    {
        tstring string("Hello {1}!", format_utf8);
        string % std::string("World");
        EXPECT_EQ(L"Hello World!", string.wstring());
    }

    {
        tstring string("Hello {1}!", format_utf8);
        string % std::wstring(L"World");
        EXPECT_EQ(L"Hello World!", string.wstring());
    }

    {
        tstring string("Hello {1} {2}!", format_utf8);
        string % tstring(L"World") % tstring(L"你好世界");
        EXPECT_EQ(L"Hello World 你好世界!", string.wstring());
    }

    {
        tstring string("Hello {1}!", format_utf8);
        string % 2;
        EXPECT_EQ(L"Hello 2!", string.wstring());
    }

    {
        tstring string("Hello {1}!", format_utf8);
        string % 2.1;
        EXPECT_EQ(L"Hello 2.1!", string.wstring());
    }

    {
        tstring string("个体和{1} 高于{2}和{3}!", format_utf8);
        string % L"互动" % L"流程" % _2str("工具", format_utf8);
        EXPECT_EQ(L"个体和互动 高于流程和工具!", string.wstring());
    }

    {
        tstring string("个体和{1} 高于{1}和{2}!", format_utf8);
        string % _2str("互动", format_utf8) % L"流程" % L"工具";
        EXPECT_EQ(L"个体和互动 高于互动和流程!", string.wstring());
    }

    {
        tstring string("个体和{1} 高于{2}和{1}!", format_utf8);
        string % L"互动" % L"流程" % L"工具";
        EXPECT_EQ(L"个体和互动 高于流程和互动!", string.wstring());
    }

    {
        tstring s1("个体和{1} 高于{2}和{1}!", format_utf8);
        tstring string = s1;
        string % L"互动" % L"流程" % L"工具";
        EXPECT_EQ(L"个体和互动 高于流程和互动!", string.wstring());
    }

    {
        tstring string, s1("个体和{1} 高于{2}和{1}!", format_utf8);
        string = s1;
        string % L"互动" % L"流程" % L"工具";
        EXPECT_EQ(L"个体和互动 高于流程和互动!", string.wstring());
    }
    

}