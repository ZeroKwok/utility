#include <gtest/gtest.h>
#include <common/acronym_for_pinyin.h>
#include <string/string_conv_easy.hpp>

using namespace util::conv::easy;

TEST(common, acronym_for_pinyin)
{
    // 中文环境下的string是GB2312
    EXPECT_EQ(util::get_acronym(_2str(L"你好")), "NH");
    EXPECT_EQ(util::get_acronym(_2str(L"返回拼音首字母缩写")), "FHPYSZMSX");
}
