#include <fstream>
#include <gtest/gtest.h>
#include "utility/string.h"
#include "utility/filesystem.h"

using namespace util;
using namespace util::fs;

// Tests for path_from_utf8
TEST(PathFromUtf8Test, ValidUtf8String) {
    std::string utf8_str = "test_directory";
    path result = path_from_utf8(utf8_str);
    EXPECT_EQ(result.string(), utf8_str);

    utf8_str = (const char*)u8"我的文档\\中文目录";
    result = path_from_utf8(utf8_str);
    EXPECT_EQ(result.wstring(), L"我的文档\\中文目录");
}

#ifdef UTILITY_SUPPORT_QT
// Tests for path_from with QString
TEST(PathFromQStringTest, ValidQString) {
    QString str = "test_directory";
    path result = path_from(str);
    EXPECT_EQ(result.string(), str.toStdString());

    str = qstr(L"我的文档\\中文目录");
    result = path_from(str);
    EXPECT_EQ(result.wstring(), L"我的文档\\中文目录");
}
#endif

// Tests for path_from_module
TEST(PathFromModuleTest, DefaultModule) {
    path result = path_from_module();
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.filename().string(), "utility_test.exe");
}

TEST(PathFromModuleTest, ModuleWithError) {
    std::error_code ec;
    path result = path_from_module(0, ec);
    EXPECT_FALSE(result.empty());
    EXPECT_FALSE(ec);
}

// Tests for path_from_module_dir
TEST(PathFromModuleDirTest, DefaultModule) {
    path result = path_from_module_dir();
    EXPECT_FALSE(result.empty());

    result = path_from_module_dir(0, "log");
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.filename().string(), "log");

    result = path_from_module_dir(0, "../../log");
    EXPECT_TRUE(result.string().find("..") == std::string::npos);

#if OS_WIN
    EXPECT_TRUE(result.string().find("/") == std::string::npos);
#else
    EXPECT_TRUE(result.string().find("/") == std::string::npos);
#endif
}

TEST(PathFromModuleDirTest, ModuleWithError) {
    std::error_code ec;
    path result = path_from_module_dir(0, ec);
    EXPECT_FALSE(result.empty());
    EXPECT_FALSE(ec);
}

// Tests for path_from_temp
TEST(PathFromTempTest, TempPath) {
    path result = path_from_temp();
    EXPECT_FALSE(result.empty());

    result = path_from_temp(L"log");
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.filename().string(), "log");

    result = path_from_temp("../../log");
    EXPECT_TRUE(result.string().find("..") == std::string::npos);
}

TEST(PathFromTempTest, TempPathWithError) {
    std::error_code ec;
    path result = path_from_temp(ec);
    EXPECT_FALSE(result.empty());
    EXPECT_FALSE(ec);
}

// Tests for path_from_home
TEST(PathFromHomeTest, HomePath) {
    path result = path_from_home();
    EXPECT_FALSE(result.empty());

    result = path_from_home(L"log");
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.filename().string(), "log");
}

TEST(PathFromHomeTest, HomePathWithError) {
    std::error_code ec;
    path result = path_from_home(ec);
    EXPECT_FALSE(result.empty());
    EXPECT_FALSE(ec);
}

// Tests for path_is_writable
TEST(PathIsWritableTest, WritablePath) {
    path temp_file = path_from_temp("utility/test_file_001.txt");
    std::ofstream(temp_file).close();
    EXPECT_FALSE(path_is_writable(temp_file));
    remove(temp_file);
    
    EXPECT_TRUE(path_is_writable(path_from_temp("utility")));
}

TEST(PathIsWritableTest, NonWritablePath) {
    // TODO
#if OS_WIN
    char szSysPath[MAX_PATH] = {};
    GetSystemDirectoryA(szSysPath, MAX_PATH);

    path non_writable_path = szSysPath;
    EXPECT_FALSE(path_is_writable(non_writable_path));
#else
    path non_writable_path = "/root/test_file.txt";  // Assumes running as non-root user
    EXPECT_FALSE(path_is_writable(non_writable_path));
#endif
}

// Tests for filename_increment
TEST(FilenameIncrementTest, IncrementFilename) {
    path filename = "log.txt";
    path incremented = filename_increment(filename);
    EXPECT_EQ(incremented.string(), "log(1).txt");

    // TODO:
}

// Tests for filename_trim
TEST(FilenameTrimTest, TrimFilename) {
    std::string filename = "nul";
    std::string trimmed = filename_trim(filename);
    EXPECT_EQ(trimmed, "(nul)");

    EXPECT_EQ(filename_trim("nul"), "(nul)");
    EXPECT_EQ(filename_trim("*"), "_");
    EXPECT_EQ(filename_trim("|"), "_");
    EXPECT_EQ(filename_trim("aux"), "(aux)");
    EXPECT_EQ(filename_trim("."),   "(.)");
    EXPECT_EQ(filename_trim(".."),  "(..)");
    EXPECT_EQ(filename_trim("..."),  "(...)");
    EXPECT_EQ(filename_trim("...."),  "(....)");
    EXPECT_EQ(filename_trim("………………………………..."),  "………………………………");
    EXPECT_EQ(filename_trim("……………………………….."),  "………………………………");
    EXPECT_EQ(filename_trim("………………………………."),  "………………………………");
    EXPECT_EQ(filename_trim("………………………………"),  "………………………………");
    EXPECT_EQ(filename_trim("read/me.txt"), "readme.txt");
    EXPECT_EQ(filename_trim("readme.?txt"), "readme.txt");

    EXPECT_EQ(filename_trim("nul", "."), "(nul)");
    EXPECT_EQ(filename_trim("aux", "."), "(aux)");
    EXPECT_EQ(filename_trim("read/me.txt", "."), "read.me.txt");
    EXPECT_EQ(filename_trim("readme.?txt", "."), "readme..txt");

    EXPECT_EQ(filename_trim("nul", "11"), "(nul)");
    EXPECT_EQ(filename_trim("aux", "11"), "(aux)");
    EXPECT_EQ(filename_trim("read/me.txt", "11"), "read11me.txt");
    EXPECT_EQ(filename_trim("readme.?txt", "11"), "readme.11txt");

    EXPECT_EQ(filename_trim(L"nul", L"1111"), L"(nul)");
    EXPECT_EQ(filename_trim(L"aux", L"1111"), L"(aux)");
    EXPECT_EQ(filename_trim(L"read/me.txt", L"1111"), L"read1111me.txt");
    EXPECT_EQ(filename_trim(L"readme.?txt", L"1111"), L"readme.1111txt");
}

TEST(PathFilenameTrimTest, TrimFilename) {
    std::string filename = "nul";
    path trimmed = path_filename_trim(filename);
    EXPECT_EQ(trimmed.string(), "(nul)");

    EXPECT_EQ(path_filename_trim("nul"), "(nul)");
    EXPECT_EQ(path_filename_trim("*"), "_");
    EXPECT_EQ(path_filename_trim("|"), "_");
    EXPECT_EQ(path_filename_trim("aux"), "(aux)");
    EXPECT_EQ(path_filename_trim("."),   "(.)");
    EXPECT_EQ(path_filename_trim(".."),  "(..)");
    EXPECT_EQ(path_filename_trim("..."),  "(...)");
    EXPECT_EQ(path_filename_trim("...."),  "(....)");
    EXPECT_EQ(path_filename_trim(L"………………………………..."),  L"………………………………");
    EXPECT_EQ(path_filename_trim(L"……………………………….."),  L"………………………………");
    EXPECT_EQ(path_filename_trim(L"………………………………."),  L"………………………………");
    EXPECT_EQ(path_filename_trim(L"………………………………"),  L"………………………………");
    EXPECT_EQ(path_filename_trim("read/me.txt"), "read/me.txt");
    EXPECT_EQ(path_filename_trim("readme.?txt"), "readme.txt");

    EXPECT_EQ(path_filename_trim("nul", "."), "(nul)");
    EXPECT_EQ(path_filename_trim("aux", "."), "(aux)");
    EXPECT_EQ(path_filename_trim("read/me*.txt", "."), "read/me..txt");
    EXPECT_EQ(path_filename_trim("readme.?txt", "."), "readme..txt");

    EXPECT_EQ(path_filename_trim("nul", "11"), "(nul)");
    EXPECT_EQ(path_filename_trim("aux", "11"), "(aux)");
    EXPECT_EQ(path_filename_trim("read/me.txt", "11"), "read/me.txt");
    EXPECT_EQ(path_filename_trim("readme.?txt", "11"), "readme.11txt");

    EXPECT_EQ(path_filename_trim(L"nul", "1111"), "(nul)");
    EXPECT_EQ(path_filename_trim(L"aux", "1111"), "(aux)");
    EXPECT_EQ(path_filename_trim(L"read/me.txt", "1111"), "read/me.txt");
    EXPECT_EQ(path_filename_trim(L"readme.?txt", "1111"), "readme.1111txt");
}

#if OS_WIN
TEST(FilenameTrimTest, TrimFilenameWithPlaceholder) {
    path path1, path2;
    std::error_code ecode;
    {
        path1 = win::path_from_sysdir(FOLDERID_Desktop, ecode);
        EXPECT_FALSE(ecode);
        EXPECT_FALSE(path1.empty());

        path2 = win::path_from_sysdir(CSIDL_DESKTOP, ecode);
        EXPECT_FALSE(ecode);
        EXPECT_FALSE(path2.empty());

        EXPECT_TRUE(path1 == path2);
    }

    {
        path1 = win::path_from_sysdir(FOLDERID_Documents, ecode);
        EXPECT_FALSE(ecode);
        EXPECT_FALSE(path1.empty());

        path2 = win::path_from_sysdir(CSIDL_PERSONAL, ecode);
        EXPECT_FALSE(ecode);
        EXPECT_FALSE(path2.empty());
        EXPECT_TRUE(path1 == path2);
    }
}
#endif

TEST(PathFromSysdirTest, PathFromSysdir) {
    std::string filename = "read/me.txt";
    std::string trimmed = filename_trim(filename, ".");
    EXPECT_EQ(trimmed, "read.me.txt");
}