#include <gtest/gtest.h>
#include <platform/console_win.h>

#if OS_WIN

using namespace util;
TEST(console_win, output)
{
    util::win::output("console_win::output()\n");
    util::win::output("console_win::output() STD_ERROR, COLOR_RED\n", 
        util::win::STD_ERROR, util::win::COLOR_RED);
}

TEST(console_win, is_redirect)
{
    EXPECT_FALSE(util::win::is_redirect());
}

TEST(console_win, cursor_pos)
{
    auto pos = util::win::cursor_pos();

    EXPECT_FALSE(pos.x == 0 && pos.y == 0);

    std::cout << "pos.x: " << pos.x  << " pos.y: " << pos.y << std::endl;
}

#endif