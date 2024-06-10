#include <fstream>
#include <gtest/gtest.h>
#include "utility/filesystem.h"

using namespace util::fs;

// Tests for path_from_utf8
TEST(PathFromUtf8Test, ValidUtf8String) {
    std::string utf8_str = "test_directory";
    path result = path_from_utf8(utf8_str);
    EXPECT_EQ(result.string(), utf8_str);
}

#ifdef UTILITY_SUPPORT_QT
// Tests for path_from with QString
TEST(PathFromQStringTest, ValidQString) {
    QString qstr = "test_directory";
    path result = path_from(qstr);
    EXPECT_EQ(result.string(), qstr.toStdString());
}
#endif

// Tests for path_from_module
TEST(PathFromModuleTest, DefaultModule) {
    path result = path_from_module();
    EXPECT_FALSE(result.empty());
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
}

TEST(PathFromHomeTest, HomePathWithError) {
    std::error_code ec;
    path result = path_from_home(ec);
    EXPECT_FALSE(result.empty());
    EXPECT_FALSE(ec);
}

// Tests for path_is_writable
TEST(PathIsWritableTest, WritablePath) {
    path temp_file = path_from_temp("test_file.txt");
    std::ofstream(temp_file).close();
    EXPECT_TRUE(path_is_writable(temp_file));
    remove(temp_file);
}

TEST(PathIsWritableTest, NonWritablePath) {
    // TODO
    // path non_writable_path = "/root/test_file.txt";  // Assumes running as non-root user
    // EXPECT_FALSE(path_is_writable(non_writable_path));
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
}

TEST(FilenameTrimTest, TrimFilenameWithPlaceholder) {
    std::string filename = "read/me.txt";
    std::string trimmed = filename_trim(filename, ".");
    EXPECT_EQ(trimmed, "read.me.txt");
}