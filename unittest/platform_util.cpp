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


#if OS_WIN

TEST(platform, wsystem_name)
{
    auto name = win::wsystem_name();
    auto verw = win::get_osversionex_w();

    EXPECT_FALSE(name.empty());
    EXPECT_TRUE(name.find(L"Windows") != name.npos);

    std::wcout << name << std::endl;
    std::wcout << "dwMajorVersion: " << verw.dwMajorVersion  << " dwBuildNumber: " << verw.dwBuildNumber << std::endl;
}

#endif