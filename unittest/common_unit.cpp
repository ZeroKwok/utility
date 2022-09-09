#include <gtest/gtest.h>
#include <common/unit.h>

TEST(common_unit, bytes_add_suffix)
{
    std::string tset1 = util::bytes_add_suffix(2519736);
    EXPECT_EQ(tset1, "2.40MB");
    std::string tset2 = util::bytes_add_suffix(228163026944);
    EXPECT_EQ(tset2, "212.49GB");
    std::string tset3 = util::bytes_add_suffix(75);
    EXPECT_EQ(tset3, "75B");
    std::string tset4 = util::bytes_add_suffix(1024, 1024, "/s");
    EXPECT_EQ(tset4, "1.00kB/s");

    std::wstring tset5 = util::wbytes_add_suffix(2519736);
    EXPECT_EQ(tset5, L"2.40MB");
}

TEST(common_unit, seconds_add_suffix)
{
    std::string tset5 = util::seconds_add_suffix(120);
    EXPECT_EQ(tset5, "2Min");
    std::string tset6 = util::seconds_add_suffix(3);
    EXPECT_EQ(tset6, "3Sec");
    std::string tset7 = util::seconds_add_suffix(70);
    EXPECT_EQ(tset7, "1Min");
    std::string tset8 = util::seconds_add_suffix(3655);
    EXPECT_EQ(tset8, "2Hr");

    std::wstring tset9 = util::wseconds_add_suffix(3655);
    EXPECT_EQ(tset9, L"2Hr");
}

TEST(common_unit, duration_format)
{
    std::string tset9 = util::duration_format(3655);
    EXPECT_EQ(tset9, "01:00:55");

    std::string tseta = util::duration_format(70);
    EXPECT_EQ(tseta, "01:10");

    std::string tsetb = util::duration_format(3);
    EXPECT_EQ(tsetb, "00:03");

    std::string tsetc = util::duration_format(75124, "-");
    EXPECT_EQ(tsetc, "20-52-04");

    std::wstring tsetd = util::wduration_format(75124, L"-");
    EXPECT_EQ(tsetd, L"20-52-04");
}
