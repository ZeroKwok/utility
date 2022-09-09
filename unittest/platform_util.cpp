#include <gtest/gtest.h>
#include <platform/platform_util.h>

using namespace util;

TEST(platform, is_little_endian)
{
#if CPU_LITTLE_ENDIAN
    EXPECT_TRUE(is_little_endian());
#else
    EXPECT_FALSE(is_little_endian());
#endif
}

TEST(platform, format_error)
{
    std::string message = format_error(0);

    EXPECT_FALSE(message.empty());
}
