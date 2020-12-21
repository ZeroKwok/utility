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

TEST(string_util, sformat)
{
    {
        int64_t a = 23378;
        EXPECT_EQ(
            "23378",
            util::sformat("%lld", a));
    }

    {
        int64_t a = 23378;
        EXPECT_EQ(
            L"23378",
            util::sformat(L"%lld", a));
    }


    {
        const char buf[] =
            "参数必须为一个指针，指向包含以初始迁移状态开始的，"\
            "多字节字符序列的字符数组首元素，数组会被转换成宽字符数组，"\
            "如同通过以零初始化转换状态调用 %s 。 精度指定写入的最大宽字符数。"\
            "若未指定精度，则写每个宽字符直到而不含首个空终止符。 若使用 %c 指定符，"\
            "则参数必须是指向 %s 数组首元素的指针。";

        std::string str = util::sformat(buf, "mbrtowc", 'l', "wchar_t");

        std::cout << str << std::endl;

        EXPECT_TRUE(str.size() > strlen(buf));
    }

    {
        const wchar_t buf[] =
            L"参数必须为一个指针，指向包含以初始迁移状态开始的，"\
            L"多字节字符序列的字符数组首元素，数组会被转换成宽字符数组，"\
            L"如同通过以零初始化转换状态调用 %ls 。 精度指定写入的最大宽字符数。"\
            L"若未指定精度，则写每个宽字符直到而不含首个空终止符。 若使用 %lc 指定符，"\
            L"则参数必须是指向 %ls 数组首元素的指针。";

        std::wstring str = util::sformat(buf, L"mbrtowc", L'l', L"wchar_t");

        std::wcout << str << std::endl;

        int a = wcslen(buf);
        int b = str.size();

        EXPECT_TRUE(str.size() > wcslen(buf));
    }
   
    {
        int a = 10;
        double d = 100.f;
        EXPECT_EQ(
            "10*100.00=1000.00",
            util::sformat("%d*%.2lf=%.2lf", a, d, a * d));
    }
}