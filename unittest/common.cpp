#include <gtest/gtest.h>
#include <common/digest.hpp>
#include <common/acronym_for_pinyin.h>
#include <platform/console_win.h>
#include <string/string_conv_easy.hpp>
#include <string/string_util.h>

using namespace util::conv::easy;

TEST(common, acronym_for_pinyin)
{
    // 中文环境下的string是GB2312
    EXPECT_EQ(util::get_acronym(_2str(L"你好")), "NH");
    EXPECT_EQ(util::get_acronym(_2str(L"返回拼音首字母缩写")), "FHPYSZMSX");
}

TEST(common, digest)
{
    std::vector<util::fpath> fileList = {
        util::path_append(util::path_find_parent(util::path_find_parent(__FILE__)), "utility.hpp"),
        util::path_append(util::path_find_parent(util::path_find_parent(__FILE__)), "link.hpp"),
        LR"(T:\test\fonetool.dmp)",
        LR"(\\192.168.4.230\共享文件夹\------------项目------------\FoneTool\测试样本\big videos\4K_60fps_IMG_2018.MOV)",
    };
    for (auto file : fileList)
    {
        std::cout << "file: " << file.string() << std::endl;

        util::win::coord pos;
        auto call = [&](util::fsize processed, util::fsize total) -> bool 
        {
            util::win::cursor_goto(pos);
            auto progress = processed / double(total) * 100;
            std::cout << util::sformat("%05.2f%%  ", progress);
            return true;
        };

        pos = util::win::cursor_pos();
        auto md5 = (std::string)util::file_md5_digest(file, 1024*512, call);
        std::cout << "- md5: " << util::bytes_into_hex(md5) << std::endl;

        pos = util::win::cursor_pos();
        auto sha1 = (std::string)util::file_sha1_digest(file, 1024 * 512, call);
        std::cout << "- sha1: " << util::bytes_into_hex(sha1) << std::endl;
    }
}
