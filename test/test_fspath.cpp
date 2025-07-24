#include <fstream>
#include <string_view>
#include "gtest/gtest.h"
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

    auto filename = result.filename().string();
    std::vector<std::string_view> files = {
#if OS_WIN
        "utility_test.exe", "utility_testd.exe"
#else
        "utility_test", "utility_testd"
#endif
    };
    auto matched = std::any_of(files.begin(), files.end(),
        [&](const std::string_view& item) {
            return item == filename;
        });
    EXPECT_TRUE(matched);
}

TEST(PathFromModuleTest, ModuleWithError) {
    std::error_code ec;
    path result = path_from_module(0, ec);
    EXPECT_FALSE(result.empty());
    EXPECT_FALSE(ec);

    EXPECT_TRUE(result.has_parent_path());
    EXPECT_TRUE(result.has_root_directory());
    EXPECT_TRUE(result.has_filename());
    EXPECT_TRUE(result.is_absolute());
}

// Tests for path_from_module_dir
TEST(PathFromModuleDirTest, DefaultModule) {
    path result = path_from_module_dir();
    EXPECT_FALSE(result.empty());

    result = path_from_module_dir(0, "log");
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.filename().string(), "log");

    // 返回的路径应该被正规化
    result = path_from_module_dir(0, "../../log");
    EXPECT_TRUE(result.string().find("..") == std::string::npos);

    EXPECT_TRUE(result.has_parent_path());
    EXPECT_TRUE(result.has_root_directory());
    EXPECT_TRUE(result.is_absolute());

#if OS_WIN
    // Windows 中文件分隔符应该被替换为 '\'
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
    path temp_file = path_from_temp("test_file_001.txt");
    std::ofstream(temp_file).close();
    auto exist = exists(temp_file);
    EXPECT_TRUE(exist);

    auto result = path_is_writable(temp_file);
    EXPECT_FALSE(result);
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

// Tests for path_filename_trim
TEST(PathFilenameTrimTest, DifferentInputsType)
{
    EXPECT_EQ(path_filename_trim(path("nul")), "(nul)");
    EXPECT_EQ(path_filename_trim("nul"), "(nul)");
    EXPECT_EQ(path_filename_trim(L"nul"), "(nul)");
    EXPECT_EQ(path_filename_trim(std::string("lpt2")), "(lpt2)");
    EXPECT_EQ(path_filename_trim(std::wstring(L"lpt2")), "(lpt2)");
    EXPECT_EQ(path_filename_trim("com4"), L"(com4)");
    EXPECT_EQ(path_filename_trim("lpt9", ".", false), "(lpt9)");
}

TEST(PathFilenameTrimTest, ReservedNames)
{
    EXPECT_EQ(path_filename_trim(path("nul")), "(nul)");
    EXPECT_EQ(path_filename_trim(path("con")), "(con)");
    EXPECT_EQ(path_filename_trim(path("aux")), "(aux)");
    EXPECT_EQ(path_filename_trim(path("COM1")), "(COM1)");
    EXPECT_EQ(path_filename_trim(path("lpt9")), "(lpt9)");
}

TEST(PathFilenameTrimTest, ParentPathPreserved)
{
    path original = "folder/nul";
    path trimmed = path_filename_trim(original);
    EXPECT_EQ(trimmed.parent_path(), path("folder"));
    EXPECT_EQ(trimmed.filename(), "(nul)");

    EXPECT_EQ(path_filename_trim("/etc/nginx/../nginx/./nginx.conf"), "/etc/nginx/../nginx/./nginx.conf");
    EXPECT_EQ(path_filename_trim("/etc/nginx/../nginx/./nginx.conf", "", false), "etcnginx..nginx.nginx.conf");
}

TEST(PathFilenameTrimTest, IllegalCharactersRemoval)
{
    EXPECT_EQ(path_filename_trim(path("read/me.txt"), "", false), "readme.txt");
    EXPECT_EQ(path_filename_trim(path("read<>:\"\\|?*.txt"), "", false), "read.txt");
    EXPECT_EQ(path_filename_trim(path("readme.?txt"), "", false), "readme.txt");
}

TEST(PathFilenameTrimTest, PlaceholderReplacement)
{
    EXPECT_EQ(path_filename_trim(path("read/me.txt"), ".", false), "read.me.txt");
    EXPECT_EQ(path_filename_trim(path("readme.?txt"), ".", false), "readme..txt");
    EXPECT_EQ(path_filename_trim(path("readme.?txt"), L"佔位符"), L"readme.佔位符txt");
}

TEST(PathFilenameTrimTest, TrailingDotOrSpace)
{
    EXPECT_EQ(path_filename_trim(path("trailingdot.")), "trailingdot");
    EXPECT_EQ(path_filename_trim(path("trailingspace ")), "trailingspace");

    EXPECT_EQ(path_filename_trim(path(".")), "_");
    EXPECT_EQ(path_filename_trim(path("..")), "_");
    EXPECT_EQ(path_filename_trim(path("...")), "_");
    EXPECT_EQ(path_filename_trim(path("....")), "_");

    EXPECT_EQ(path_filename_trim(path(L"………………………………...")), L"………………………………");
}

TEST(PathFilenameTrimTest, EmptyFilename)
{
    EXPECT_EQ(path_filename_trim(path("")), "");
}

// Tests for path_filename_increment
TEST(PathFilenameIncrementTest, SimpleIncrement)
{
    EXPECT_EQ(path_filename_increment("log.txt"), "log(1).txt");
    EXPECT_EQ(path_filename_increment("log(1).txt"), "log(2).txt");
    EXPECT_EQ(path_filename_increment("log(99).txt"), "log(100).txt");
}

TEST(PathFilenameIncrementTest, IgnoreExtension)
{
    EXPECT_EQ(path_filename_increment("log.txt", true), "log.txt(1)");
    EXPECT_EQ(path_filename_increment("log.txt(5)", true), "log.txt(6)");
}

TEST(PathFilenameIncrementTest, NoExtension)
{
    EXPECT_EQ(path_filename_increment("log"), "log(1)");
    EXPECT_EQ(path_filename_increment("log(7)"), "log(8)");
}

TEST(PathFilenameIncrementTest, NestedDirectory)
{
    path original = "dir/subdir/file.txt";
    path incremented = path_filename_increment(original);
    EXPECT_EQ(incremented.parent_path(), path("dir/subdir"));
    EXPECT_EQ(incremented.filename(), "file(1).txt");
}

TEST(PathFilenameIncrementTest, MultipleSuffixPattern)
{
    EXPECT_EQ(path_filename_increment("data(2)(3).log"), "data(2)(4).log");
    EXPECT_EQ(path_filename_increment("abc(100).txt"), "abc(101).txt");
}

TEST(PathFilenameIncrementTest, NestedParentheses)
{
    EXPECT_EQ(path_filename_increment("file((1)).txt"), "file((1))(1).txt");
    EXPECT_EQ(path_filename_increment("image(name(1)).jpg"), "image(name(1))(1).jpg");
}

TEST(PathFilenameIncrementTest, IncompleteParentheses)
{
    EXPECT_EQ(path_filename_increment("file(.txt"), "file((1).txt");
    EXPECT_EQ(path_filename_increment("file(1.txt"), "file(1(1).txt");
    EXPECT_EQ(path_filename_increment("file).txt"), "file)(1).txt");
}

TEST(PathFilenameIncrementTest, UnicodeNames)
{
    // UTF-8 (or native wide path, depending on OS)
    EXPECT_EQ(path_filename_increment("照片(3).png"), "照片(4).png");
    EXPECT_EQ(path_filename_increment("文档.txt"), "文档(1).txt");
    EXPECT_EQ(path_filename_increment("Пример(7).doc"), "Пример(8).doc");
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
