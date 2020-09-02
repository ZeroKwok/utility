#include <gtest/gtest.h>
#include <string/tstring.hpp>
#include <string/string_conv_easy.hpp>

TEST(tstring, wstring)
{
    util::tstring string("Hello World");
    EXPECT_EQ(L"Hello World", string.wstring());
}
