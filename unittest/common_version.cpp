#include <gtest/gtest.h>
#include <common/version.h>

TEST(common_version, version)
{
    EXPECT_EQ(util::version_generate(2, 0, 0, 1), 0x02000001);
    EXPECT_EQ(util::version_generate(1, 2, 3, 4), 0x01020304);

    util::product_version version = { 2, 0, 0, 1 };
    util::product_version version1 = { 1, 2, 3, 4 };
    util::product_version version2 = { 1, 2, 3, 0 };
    EXPECT_EQ(util::version_into_value(version), 0x02000001);
    EXPECT_EQ(util::version_into_value(version1), 0x01020304);

    EXPECT_EQ(util::version_into_string(version), "2.0.0.1");
    EXPECT_EQ(util::version_into_string(version1), "1.2.3.4");
    EXPECT_EQ(util::version_into_string(version2), "1.2.3");

    EXPECT_EQ(util::version_into_wstring(version), L"2.0.0.1");
    EXPECT_EQ(util::version_into_wstring(version1), L"1.2.3.4");
    EXPECT_EQ(util::version_into_wstring(version2), L"1.2.3");

    EXPECT_TRUE(util::version_from_string("2.0.0.1") == version);
    EXPECT_TRUE(util::version_from_string("1.2.3.4") == version1);

    EXPECT_TRUE(util::version_from_string(L"2.0.0.1") == version);
    EXPECT_TRUE(util::version_from_string(L"1.2.3.4") == version1);
}
