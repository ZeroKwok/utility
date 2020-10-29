#include <gtest/gtest.h>
#include <platform/mini_dump.hpp>
#include <filesystem/path_util.hpp>

TEST(platform, mini_dump)
{
    util::win::mini_dump::instance().init();
    util::win::mini_dump::instance().set_crash_handler([]() 
    {
        std::cout << "Program crash handling..." << std::endl;
    });

    util::fpath file = util::path_from_module_dir("dump\\dump.dump");
    
    if (util::file_exist(file))
        util::file_remove(file);
    util::win::mini_dump::create_dump(file);

    EXPECT_TRUE(util::file_exist(file));
    
    // ´¥·¢Òì³£
    *((int*)0) = 3;
}