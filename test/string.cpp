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
        util::tstring string("���ǵ��������ǳ���~~~");
        EXPECT_EQ(u8"���ǵ��������ǳ���~~~", string.utf8());
        EXPECT_EQ("���ǵ��������ǳ���~~~", string.string());
        EXPECT_EQ(L"���ǵ��������ǳ���~~~", string.wstring());
    }

    {
        util::tstring string(u8"���ǵ��������ǳ���~~~", util::tstring::format_utf8);
        EXPECT_EQ(u8"���ǵ��������ǳ���~~~", string.utf8());
        EXPECT_EQ("���ǵ��������ǳ���~~~", string.string());
        EXPECT_EQ(L"���ǵ��������ǳ���~~~", string.wstring());
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
        util::tstring string(L"���ǵ��������ǳ���~~~");
        EXPECT_EQ(u8"���ǵ��������ǳ���~~~", string.utf8());
        EXPECT_EQ("���ǵ��������ǳ���~~~", string.string());
        EXPECT_EQ(L"���ǵ��������ǳ���~~~", string.wstring());
    }

    {
        util::tstring string(std::string(u8"���ǵ��������ǳ���~~~"), util::tstring::format_utf8);
        EXPECT_EQ(u8"���ǵ��������ǳ���~~~", string.utf8());
        EXPECT_EQ("���ǵ��������ǳ���~~~", string.string());
        EXPECT_EQ(L"���ǵ��������ǳ���~~~", string.wstring());
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
        util::tstring string(std::string("���ǵ��������ǳ���~~~"));
        EXPECT_EQ(u8"���ǵ��������ǳ���~~~", string.utf8());
        EXPECT_EQ("���ǵ��������ǳ���~~~", string.string());
        EXPECT_EQ(L"���ǵ��������ǳ���~~~", string.wstring());
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
        util::tstring string(std::wstring(L"���ǵ��������ǳ���~~~"));
        EXPECT_EQ(u8"���ǵ��������ǳ���~~~", string.utf8());
        EXPECT_EQ("���ǵ��������ǳ���~~~", string.string());
        EXPECT_EQ(L"���ǵ��������ǳ���~~~", string.wstring());
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
        util::tstring s1(u8"���ǵ��������ǳ���~~~", util::tstring::format_utf8);
        util::tstring string = s1;
        EXPECT_EQ(L"���ǵ��������ǳ���~~~", string.wstring());
    }

    {
        util::tstring s1(u8"���ǵ��������ǳ���~~~", util::tstring::format_utf8);
        util::tstring string = std::move(s1);
        EXPECT_EQ(L"���ǵ��������ǳ���~~~", string.wstring());
    }

    {
        util::tstring s1(u8"���ǵ��������ǳ���~~~", util::tstring::format_utf8);
        util::tstring string(s1);
        EXPECT_EQ(L"���ǵ��������ǳ���~~~", string.wstring());
    }

    {
        util::tstring s1(u8"���ǵ��������ǳ���~~~", util::tstring::format_utf8);
        util::tstring string(std::move(s1));
        EXPECT_EQ(L"���ǵ��������ǳ���~~~", string.wstring());
    }

    {
        util::tstring string, s1(u8"���ǵ��������ǳ���~~~", util::tstring::format_utf8);
        string = s1;
        EXPECT_EQ(L"���ǵ��������ǳ���~~~", string.wstring());
    }

    {
        util::tstring string, s1(u8"���ǵ��������ǳ���~~~", util::tstring::format_utf8);
        string = std::move(s1);
        EXPECT_EQ(L"���ǵ��������ǳ���~~~", string.wstring());
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
        string % util::tstring(L"World") % util::tstring(L"�������");
        EXPECT_EQ(L"Hello World �������!", string.wstring());
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
        util::tstring string(u8"�����{1} ����{2}��{3}!", util::tstring::format_utf8);
        string % "����" % "����" % L"����";
        EXPECT_EQ(L"����ͻ��� �������̺͹���!", string.wstring());
    }

    {
        util::tstring string(u8"�����{1} ����{1}��{2}!", util::tstring::format_utf8);
        string % "����" % "����" % L"����";
        EXPECT_EQ(L"����ͻ��� ���ڻ���������!", string.wstring());
    }

    {
        util::tstring string(u8"�����{1} ����{2}��{1}!", util::tstring::format_utf8);
        string % "����" % "����" % L"����";
        EXPECT_EQ(L"����ͻ��� �������̺ͻ���!", string.wstring());
    }

    {
        util::tstring s1(u8"�����{1} ����{2}��{1}!", util::tstring::format_utf8);
        util::tstring string = s1;
        string % "����" % "����" % L"����";
        EXPECT_EQ(L"����ͻ��� �������̺ͻ���!", string.wstring());
    }

    {
        util::tstring string, s1(u8"�����{1} ����{2}��{1}!", util::tstring::format_utf8);
        string = s1;
        string % "����" % "����" % L"����";
        EXPECT_EQ(L"����ͻ��� �������̺ͻ���!", string.wstring());
    }
}