#include <gtest/gtest.h>
#include <filesystem/path_util.h>

#if OS_WIN
#   include <shlobj.h>
#endif

TEST(path_util, path_from_module)
{
    util::fpath  path, path2;
    util::ferror ferr;

    path = util::path_from_module(0, ferr);
    EXPECT_FALSE(ferr);
    EXPECT_FALSE(path.empty());

    auto exist = util::file_exist(path, ferr);
    EXPECT_FALSE(ferr);
    EXPECT_TRUE(exist);

    path2 = util::path_from_module_dir(0, ferr);
    EXPECT_FALSE(ferr);
    EXPECT_TRUE(path.find(path2) != path.npos && path.size() > path2.size());

    path  = path2;
    path2 = util::path_from_module_dir("config", ferr);
    EXPECT_FALSE(ferr);
    EXPECT_TRUE(path2.find(path) != path.npos && path2.size() > path.size());

}

TEST(path_util, path_from_temp)
{
    util::fpath  path, path2;
    util::ferror ferr;

    path = util::path_from_temp(ferr);
    EXPECT_FALSE(ferr);
    EXPECT_FALSE(path.empty());

    auto exist = util::file_exist(path, ferr);
    EXPECT_FALSE(ferr);
    EXPECT_TRUE(exist);

    path2 = util::path_from_temp("config", ferr);
    EXPECT_FALSE(ferr);
    EXPECT_TRUE(path2.find(path) != path.npos && path2.size() > path.size());
}

TEST(path_util, path_from_home)
{
    util::fpath  path, path2;
    util::ferror ferr;

    path = util::path_from_home(ferr);
    EXPECT_FALSE(ferr);
    EXPECT_FALSE(path.empty());

    auto exist = util::file_exist(path, ferr);
    EXPECT_FALSE(ferr);
    EXPECT_TRUE(exist);

    path2 = util::path_from_home("config", ferr);
    EXPECT_FALSE(ferr);
    EXPECT_TRUE(path2.find(path) != path.npos && path2.size() > path.size());
}

TEST(path_util, path_from_sysdir)
{
#if OS_WIN
    util::fpath  path, path2;
    util::ferror ferr;

    path = util::path_from_sysdir(FOLDERID_Desktop, ferr);
    EXPECT_FALSE(ferr);
    EXPECT_FALSE(path.empty());

    path2 = path_from_sysdir(CSIDL_DESKTOP, ferr);
    EXPECT_FALSE(ferr);
    EXPECT_FALSE(path2.empty());

    EXPECT_TRUE(path == path2);
#endif
}

TEST(path_util, path_is_root)
{
    // posix
    EXPECT_TRUE(util::path_is_root("/"));
    EXPECT_TRUE(util::path_is_root(L"/"));
    EXPECT_FALSE(util::path_is_root("./"));
    EXPECT_FALSE(util::path_is_root("/etc"));

    // windows
    EXPECT_TRUE(util::path_is_root("C:"));
    EXPECT_TRUE(util::path_is_root("c:/"));
    EXPECT_TRUE(util::path_is_root("C:\\"));
    EXPECT_TRUE(util::path_is_root("\\\\?\\C:"));
    EXPECT_TRUE(util::path_is_root(L"\\\\?\\C:\\"));
    EXPECT_FALSE(util::path_is_root("\\"));
    EXPECT_FALSE(util::path_is_root("E:\\folder"));
    EXPECT_FALSE(util::path_is_root("E:\\folder"));

    // UNC
    EXPECT_TRUE(util::path_is_root("\\\\servername\\share"));
    EXPECT_TRUE(util::path_is_root("\\\\servername\\share\\"));
    EXPECT_TRUE(util::path_is_root("\\\\?\\UNC\\servername\\share\\"));
    EXPECT_FALSE(util::path_is_root("\\\\servername\\share\\file"));
    EXPECT_FALSE(util::path_is_root("\\\\servername\\"));
}

TEST(path_util, path_is_unc)
{
    EXPECT_TRUE(util::path_is_unc(R"(\\servername\share)"));
    EXPECT_TRUE(util::path_is_unc(R"(\\servername\share\folder)"));
    EXPECT_TRUE(util::path_is_unc(R"(\\?\UNC\servername\share)"));
    EXPECT_FALSE(util::path_is_unc(R"(\\servername\)"));
}

TEST(path_util, path_is_url)
{
    EXPECT_TRUE(util::path_is_url(R"(https://www.baidu.com/s?ie=utf-8)"));
    EXPECT_TRUE(util::path_is_url(R"(ftp://test:test@192.168.0.1:21/profile)"));
    EXPECT_TRUE(util::path_is_url(R"(file:///G:/local/cmake/index.html)"));
    EXPECT_FALSE(util::path_is_url(R"(\\servername\share\)"));
}

TEST(path_util, path_is_win_style)
{
    EXPECT_TRUE(util::path_is_win_style(R"(C:)"));
    EXPECT_TRUE(util::path_is_win_style(R"(c:\)"));
    EXPECT_TRUE(util::path_is_win_style(R"(C:/)"));
    EXPECT_TRUE(util::path_is_win_style(R"(\\?\C:/)"));
    EXPECT_TRUE(util::path_is_win_style(R"(.\config)"));
    EXPECT_TRUE(util::path_is_win_style(R"(\PragramData)"));

    EXPECT_FALSE(util::path_is_win_style(R"(./)"));
    EXPECT_FALSE(util::path_is_win_style(R"(/etc)"));
}

TEST(path_util, path_is_win_long_path)
{
    EXPECT_TRUE(util::path_is_win_long_path(R"(\\?\D:\very)"));
    EXPECT_TRUE(util::path_is_win_long_path(R"(\\?\UNC\server\share)"));
    EXPECT_FALSE(util::path_is_win_long_path(R"(D:\very)"));
    EXPECT_FALSE(util::path_is_win_long_path(R"(\\server\share)"));
}

TEST(path_util, path_is_unc_style)
{
    EXPECT_TRUE(util::path_is_unc_style(R"(\\?\UNC\server\share)"));
    EXPECT_TRUE(util::path_is_unc_style(R"(\\server\share)"));

    // 暂不支持
    EXPECT_FALSE(util::path_is_unc_style(R"(//server/share)"));
}

TEST(path_util, path_is_writable)
{
    util::ferror ferr;

#if OS_POSIX
    EXPECT_TRUE(util::path_is_writable(util::path_from_temp(ferr), ferr));
    EXPECT_FALSE(util::path_is_writable("/etc", ferr));

#elif OS_WIN
    EXPECT_TRUE(util::path_is_writable(util::path_from_temp(ferr), ferr));
    EXPECT_FALSE(util::path_is_writable(util::path_from_sysdir(CSIDL_SYSTEMX86, ferr), ferr));
#endif
}

TEST(path_util, path_append)
{
#if OS_WIN
    EXPECT_EQ(util::path_append("C:\\", "\\"), "C:\\");
    EXPECT_EQ(util::path_append("C:\\", "Program Files"), "C:\\Program Files");
    EXPECT_EQ(util::path_append("C:\\Program Files", "Folder"), "C:\\Program Files\\Folder");
    EXPECT_EQ(util::path_append("C:\\Program Files", "\\\\Folder"), "C:\\Program Files\\Folder");
    EXPECT_EQ(util::path_append("C:\\Program Files", "\\\\Folder\\\\filename"), "C:\\Program Files\\Folder\\\\filename");
    EXPECT_EQ(util::path_append("", "\\Folder"), "\\Folder");
    EXPECT_EQ(util::path_append("\\\\server\\share", "folder"), "\\\\server\\share\\folder");
#endif

    EXPECT_EQ(util::path_append("", "/etc"), "/etc");
    EXPECT_EQ(util::path_append("/etc", "/"), "/etc");
    EXPECT_EQ(util::path_append("/etc", "//"), "/etc");
    EXPECT_EQ(util::path_append("/etc/", "/passwd"), "/etc/passwd");
    EXPECT_EQ(util::path_append("/etc/", "/config//init"), "/etc/config//init");
    EXPECT_EQ(util::path_append("/etc", "passwd"), "/etc/passwd");

    EXPECT_EQ(util::path_append("https://example.com", "index.html"), "https://example.com/index.html");
}

TEST(path_util, path_find_root)
{
#if OS_WIN
    EXPECT_EQ(util::path_find_root("C:\\Program Files\\windows"), "C:\\");
    EXPECT_EQ(util::path_find_root("\\\\?\\C:\\Program Files"), "\\\\?\\C:\\");
#endif

    EXPECT_EQ(util::path_find_root("/home/git"), "/");
}

TEST(path_util, path_find_parent)
{
#if OS_WIN
    EXPECT_EQ(util::path_find_parent("C:\\Program Files\\windows"), "C:\\Program Files");
    EXPECT_EQ(util::path_find_parent("\\\\192.168.0.13\\share\\folder"), "\\\\192.168.0.13\\share");
    EXPECT_EQ(util::path_find_parent("\\\\?\\C:\\Program Files"), "\\\\?\\C:\\");
    EXPECT_EQ(util::path_find_parent("\\\\?\\UNC\\server\\share"), "\\\\?\\UNC\\server\\share");
    EXPECT_EQ(util::path_find_parent("\\\\?\\UNC\\server\\share\\folder"), "\\\\?\\UNC\\server\\share");
#endif

    EXPECT_EQ(util::path_find_parent("/tmp/archive.tar.gz"), "/tmp");
    EXPECT_EQ(util::path_find_parent("/"), "/");
}

TEST(path_util, path_find_filename)
{
#if OS_WIN
    EXPECT_EQ(util::path_find_filename("C:\\Program Files\\windows"), "windows");
    EXPECT_EQ(util::path_find_filename("C:\\Program Files\\archive.tar.gz"), "archive.tar.gz");
#endif
    EXPECT_EQ(util::path_find_filename("/tmp/archive.tar.gz"), "archive.tar.gz");
}

TEST(path_util, path_find_basename)
{
#if OS_WIN
    EXPECT_EQ(util::path_find_basename("C:\\Program Files\\windows"), "windows");
    EXPECT_EQ(util::path_find_basename("C:\\Program Files\\archive.tar.gz"), "archive");
#endif

    EXPECT_EQ(util::path_find_basename("/tmp/archive.tar.gz"), "archive");
    EXPECT_EQ(util::path_find_basename("/tmp/.archive"), "");
}

TEST(path_util, path_find_extension)
{
#if OS_WIN
    EXPECT_EQ(util::path_find_extension("C:\\archive.tar.gz"), "gz");
    EXPECT_EQ(util::path_find_extension("C:\\archive.tar.gz", util::find_complete), "tar.gz");
    EXPECT_EQ(util::path_find_extension("C:\\archive.tar.gz", util::find_upper_case), "GZ");
    EXPECT_EQ(util::path_find_extension("C:\\archive.tar.gz", util::find_with_dot), ".gz");
    EXPECT_EQ(util::path_find_extension("C:\\archive.tar.gz", util::find_complete  | util::find_with_dot), ".tar.gz");
    EXPECT_EQ(util::path_find_extension("C:\\archive.tar.gz", util::find_complete  | util::find_with_dot | util::find_upper_case), ".TAR.GZ");
#endif

    EXPECT_EQ(util::path_find_extension("/home/git/archive.tar.gz"), "gz");
    EXPECT_EQ(util::path_find_extension("/home/git/archive.tar.gz", util::find_complete), "tar.gz");
    EXPECT_EQ(util::path_find_extension("/home/git/archive.tar.gz", util::find_upper_case), "GZ");
    EXPECT_EQ(util::path_find_extension("/home/git/archive.tar.gz", util::find_with_dot), ".gz");
    EXPECT_EQ(util::path_find_extension("/home/git/archive.tar.gz", util::find_complete | util::find_with_dot), ".tar.gz");
    EXPECT_EQ(util::path_find_extension("/home/git/archive.tar.gz", util::find_complete | util::find_with_dot | util::find_upper_case), ".TAR.GZ");
    EXPECT_EQ(util::path_find_extension("/etc/.git"), "git");
}

TEST(path_util, path_filename_trim)
{
    EXPECT_EQ(util::path_filename_trim("nul"), "_nul");
    EXPECT_EQ(util::path_filename_trim("aux"), "_aux");
    EXPECT_EQ(util::path_filename_trim("read/me.txt"), "readme.txt");
    EXPECT_EQ(util::path_filename_trim("readme.?txt"), "readme.txt");

    EXPECT_EQ(util::path_filename_trim("nul", "."), "_nul");
    EXPECT_EQ(util::path_filename_trim("aux", "."), "_aux");
    EXPECT_EQ(util::path_filename_trim("read/me.txt", "."), "read.me.txt");
    EXPECT_EQ(util::path_filename_trim("readme.?txt", "."), "readme..txt");

    EXPECT_EQ(util::path_filename_trim("nul", "11"), "_nul");
    EXPECT_EQ(util::path_filename_trim("aux", "11"), "_aux");
    EXPECT_EQ(util::path_filename_trim("read/me.txt", "11"), "read11me.txt");
    EXPECT_EQ(util::path_filename_trim("readme.?txt", "11"), "readme.11txt");

    EXPECT_EQ(util::path_filename_trim(L"nul", L"1111"), L"_nul");
    EXPECT_EQ(util::path_filename_trim(L"aux", L"1111"), L"_aux");
    EXPECT_EQ(util::path_filename_trim(L"read/me.txt", L"1111"), L"read1111me.txt");
    EXPECT_EQ(util::path_filename_trim(L"readme.?txt", L"1111"), L"readme.1111txt");
}

TEST(path_util, path_filename_increment)
{
    auto result_1 = util::path_filename_increment("log.txt");
    EXPECT_EQ(result_1 , "log(1).txt");

    auto result_2 = util::path_filename_increment(result_1);
    EXPECT_EQ(result_2 , "log(2).txt");

    auto result_3 = util::path_filename_increment(L"log( 1036 ).txt");
    EXPECT_EQ(result_3 , L"log(1037).txt");

    auto result_4 = util::path_filename_increment("log(a).txt");
    EXPECT_EQ(result_4 , "log(a)(1).txt");

    auto result_5 = util::path_filename_increment(L"file(中文).log");
    EXPECT_EQ(result_5 , L"file(中文)(1).log");

    auto result_6 = util::path_filename_increment(L"file(hello world)");
    EXPECT_EQ(result_6 , L"file(hello world)(1)");

    auto result_7 = util::path_filename_increment(L"./folder/filename.(4)");
    EXPECT_EQ(result_7 , L"./folder/filename(1).(4)");

    auto result_8 = util::path_filename_increment(L"./folder(1)/filename");
    EXPECT_EQ(result_8 , L"./folder(1)/filename(1)");

    auto result_9 = util::path_filename_increment(L"./folder/filename(1)-backup.txt");
    EXPECT_EQ(result_9 , L"./folder/filename(1)-backup(1).txt");

    auto result_10 = util::path_filename_increment("./folder/filename(1)-backup");
    EXPECT_EQ(result_10 , "./folder/filename(1)-backup(1)");

    auto result_11 = util::path_filename_increment(L"./folder/filename(1) .txt");
    EXPECT_EQ(result_11 , L"./folder/filename(2) .txt");

    auto result_12 = util::path_filename_increment("./folder/filename(1) ");
    EXPECT_EQ(result_12 , "./folder/filename(2) ");
}

TEST(path_util, path_filesystem)
{
#if OS_WIN
    size_t size = ::GetLogicalDriveStrings(0, nullptr);
    std::string buffer(size, 0);
    ::GetLogicalDriveStrings(buffer.length(), &buffer[0]);

    std::list<std::string> drives;
    char* p = buffer.data();
    while (*p)
    {
        drives.push_back(p);
        p += strlen(p) + 1;
    }
    
    for (auto d : drives)
    {
        util::ferror ferr;
        EXPECT_TRUE(util::path_filesystem(d, ferr) != util::none && !ferr);
    }
#endif
}
