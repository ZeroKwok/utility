#include <gtest/gtest.h>
#include <string/string_util.h>
#include <string/string_conv_easy.hpp>

//
// 这里需要注意的是, 源文件为utf-8编码
//
using namespace util;
using namespace conv::easy;

TEST(string_util, left)
{
    EXPECT_EQ("string", left("string/string_conv_easy.hpp", "/"));
    EXPECT_EQ(L"string", left(L"string/string_conv_easy.hpp", L"/"));

    EXPECT_EQ(std::string("string"), 
        left(std::string("string/string_conv_easy.hpp"), std::string("/")));
    EXPECT_EQ(std::wstring(L"string"),
        left(std::wstring(L"string/string_conv_easy.hpp"), std::wstring(L"/")));

    EXPECT_EQ(std::wstring(L""),
        left(std::wstring(L"/string_conv_easy.hpp"), std::wstring(L"/")));

    EXPECT_EQ("", left("", "/"));
}

TEST(string_util, right)
{
    EXPECT_EQ("string_conv_easy.hpp", right("string/string_conv_easy.hpp", "/"));
    EXPECT_EQ(L"string_conv_easy.hpp", right(L"string/string_conv_easy.hpp", L"/"));

    EXPECT_EQ(std::string("string_conv_easy.hpp"),
        right(std::string("string/string_conv_easy.hpp"), std::string("/")));
    EXPECT_EQ(std::wstring(L"string_conv_easy.hpp"),
        right(std::wstring(L"string/string_conv_easy.hpp"), std::wstring(L"/")));

    EXPECT_EQ(std::wstring(L""),
        right(std::wstring(L"string/"), std::wstring(L"/")));

    EXPECT_EQ("", right("", "/"));
}

TEST(string_util, in_half)
{
    {
        auto result = in_half_from_left("00123=256=144", "=");
        EXPECT_EQ(result.first, "00123");
        EXPECT_EQ(result.second, "256=144");
    }

    {
        auto result = in_half_from_left(L"00123=256=144", L"=");
        EXPECT_EQ(result.first, L"00123");
        EXPECT_EQ(result.second, L"256=144");
    }

    {
        auto result = in_half_from_right("00123=256=144", "=");
        EXPECT_EQ(result.first, "00123=256");
        EXPECT_EQ(result.second, "144");
    }

    {
        auto result = in_half_from_right(L"00123=256=144", L"=");
        EXPECT_EQ(result.first, L"00123=256");
        EXPECT_EQ(result.second, L"144");
    }
}

TEST(string_util, between)
{
    EXPECT_EQ("_conv_", 
        between("string/string_conv_easy.hpp", "string", "easy.hpp"));
    EXPECT_EQ("string_conv_easy.hpp",
        between("string/string_conv_easy.hpp", "string", "easy.hpp", contains_mark));

    EXPECT_EQ(std::string("_conv_"),
        between("string/string_conv_easy.hpp", std::string("string"), "easy.hpp"));
    EXPECT_EQ(L"string_conv_easy.hpp",
        between(std::wstring(L"string/string_conv_easy.hpp"), L"string", L"easy.hpp", contains_mark));

    EXPECT_EQ("",
        between("string/string_conv_easy.hpp", "1", "easy.hpp"));
    EXPECT_EQ("",
        between("string/string_conv_easy.hpp", "/", "-"));

    EXPECT_EQ("123",
        between("a...a123b", "a", "b"));

    EXPECT_EQ(L"工具",
        between(L"请启用“工具”->“选项”->“调试”", L"“", L"”"));
    EXPECT_EQ("工具",
        between("请启用“工具”->“选项”->“调试”", "“", "”"));

    EXPECT_EQ("ProgramData",
        between("%ProgramData%/AomeiMB", "%", "%"));
    EXPECT_EQ("Program",
        between("%Program%Data%/AomeiMB", "%", "%"));
    EXPECT_EQ("%Program%",
        between("%Program%Data%/AomeiMB", "%", "%", contains_mark));
}

TEST(string_util, start_with)
{
    EXPECT_TRUE(start_with("string_conv_easy.hpp", ""));
    EXPECT_TRUE(start_with("string_conv_easy.hpp", "string"));
    EXPECT_TRUE(start_with("string_conv_easy.hpp", "string_conv_easy.hpp"));
    EXPECT_FALSE(start_with("string_conv_easy.hpp", "string_conv_easy.hpp_"));

    EXPECT_TRUE(start_with(L"string_conv_easy.hpp", L""));
    EXPECT_TRUE(start_with(L"string_conv_easy.hpp", L"string"));
    EXPECT_TRUE(start_with(L"string_conv_easy.hpp", L"string_conv_easy.hpp"));
    EXPECT_FALSE(start_with(L"string_conv_easy.hpp", L"string_conv_easy.hpp_"));
}

TEST(string_util, end_with)
{
    EXPECT_TRUE(end_with("string_conv_easy.hpp", ""));
    EXPECT_TRUE(end_with("string_conv_easy.hpp", "easy.hpp"));
    EXPECT_TRUE(end_with("string_conv_easy.hpp", "string_conv_easy.hpp"));
    EXPECT_FALSE(end_with("string_conv_easy.hpp", ".string_conv_easy.hpp"));

    EXPECT_TRUE(end_with(L"string_conv_easy.hpp", L""));
    EXPECT_TRUE(end_with(L"string_conv_easy.hpp", L"easy.hpp"));
    EXPECT_TRUE(end_with(L"string_conv_easy.hpp", L"string_conv_easy.hpp"));
    EXPECT_FALSE(end_with(L"string_conv_easy.hpp", L".string_conv_easy.hpp"));
}

TEST(string_util, sformat)
{
    {
        int64_t a = 23378;
        EXPECT_EQ(
            "23378",
            sformat("%" PRId64, a));
    }

    {
        int64_t a = 23378;
        EXPECT_EQ(
            L"23378",
            sformat(L"%" PRId64, a));
    }

    {
        const char buf[] =
            "参数必须为一个指针，指向包含以初始迁移状态开始的，"\
            "多字节字符序列的字符数组首元素，数组会被转换成宽字符数组，"\
            "如同通过以零初始化转换状态调用 %s 。 精度指定写入的最大宽字符数。"\
            "若未指定精度，则写每个宽字符直到而不含首个空终止符。 若使用 %c 指定符，"\
            "则参数必须是指向 %s 数组首元素的指针。";

        std::string str = sformat(buf, "mbrtowc", 'l', "wchar_t");

        EXPECT_TRUE(str.size() > strlen(buf));
    }

    {
        const wchar_t buf[] =
            L"参数必须为一个指针，指向包含以初始迁移状态开始的，"\
            L"多字节字符序列的字符数组首元素，数组会被转换成宽字符数组，"\
            L"如同通过以零初始化转换状态调用 %ls 。 精度指定写入的最大宽字符数。"\
            L"若未指定精度，则写每个宽字符直到而不含首个空终止符。 若使用 %lc 指定符，"\
            L"则参数必须是指向 %ls 数组首元素的指针。";

        std::wstring str = sformat(buf, L"mbrtowc", L'l', L"wchar_t");

        EXPECT_TRUE(str.size() > wcslen(buf));
    }
   
    {
        int a = 10;
        double d = 100.f;
        EXPECT_EQ(
            "10*100.00=1000.00",
            sformat("%d*%.2lf=%.2lf", a, d, a * d));
    }
}