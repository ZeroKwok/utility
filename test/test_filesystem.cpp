#include <fstream>
#include <gtest/gtest.h>
#include "utility/error.h"
#include "utility/string.h"
#include "utility/filesystem.h"

using namespace util;
using namespace util::fs;

// Helper function to create temporary file for testing
path create_temp_file(const std::string& name) {
    path temp_file = path_from_temp(name);
    std::ofstream(temp_file).close();
    return temp_file;
}

// Tests for open
TEST(FileOperationsTest, OpenFileSuccess) {
    path temp_file = create_temp_file("test_file_002.txt");
    file f = open(temp_file, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);
    close(f);
}

TEST(FileOperationsTest, OpenFileFailure) {
    std::error_code ec;
    file f = fs::open("/nonexistent/path/file.txt", O_RDWR | O_CREAT, ec);
    ASSERT_EQ(f, nullptr);
    ASSERT_TRUE(ec);
}

// Tests for close
TEST(FileOperationsTest, CloseFile) {
    path temp_file = create_temp_file("test_file_003.txt");
    file f = open(temp_file, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);
    close(f);
    // No direct way to test close, but no exceptions should be thrown
}

// Tests for read
TEST(FileOperationsTest, ReadFile) {
    path temp_file = create_temp_file("test_file_004.txt");
    file f = open(temp_file, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);
    const char* write_data = "test data";
    write(f, write_data, strlen(write_data));
    seek(f, 0, SEEK_SET);

    char read_data[10];
    size bytes_read = read(f, read_data, 9);
    read_data[bytes_read] = '\0';
    EXPECT_EQ(std::string(read_data), "test data");
    close(f);
}

TEST(FileOperationsTest, ReadFileError) {
    path temp_file = create_temp_file("test_file_005.txt");
    file f = open(temp_file, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);

    std::error_code ec;
    char read_data[10];
    size bytes_read = read(f, read_data, 9, ec);
    ASSERT_EQ(bytes_read, 0);  // File is empty
    ASSERT_FALSE(ec);          // No error since reaching EOF is not an error
    close(f);
}

// Tests for write
TEST(FileOperationsTest, WriteFile) {
    path temp_file = create_temp_file("test_file_00.txt");
    file f = open(temp_file, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);

    const char* write_data = "test data";
    size bytes_written = write(f, write_data, strlen(write_data));
    EXPECT_EQ(bytes_written, strlen(write_data));

    seek(f, 0, SEEK_SET);
    char read_data[10];
    size bytes_read = read(f, read_data, 9);
    read_data[bytes_read] = '\0';
    EXPECT_EQ(std::string(read_data), "test data");
    close(f);
}

// Tests for seek and tell
TEST(FileOperationsTest, SeekAndTell) {
    path temp_file = create_temp_file("test_file_007.txt");
    file f = open(temp_file, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);

    const char* write_data = "test data";
    write(f, write_data, strlen(write_data));

    seek(f, 5, SEEK_SET);
    EXPECT_EQ(tell(f), 5);

    char read_data[5];
    size bytes_read = read(f, read_data, 4);
    read_data[bytes_read] = '\0';
    EXPECT_EQ(std::string(read_data), "data");
    close(f);
}

// Tests for file_size
TEST(FileOperationsTest, FileSize) {
    path temp_file = create_temp_file("test_file_008.txt");
    file f = open(temp_file, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);

    const char* write_data = "test data";
    write(f, write_data, strlen(write_data));

    EXPECT_EQ(file_size(f), strlen(write_data));
    close(f);
}

// Tests for time
TEST(FileOperationsTest, FileTime) {
    path temp_file = create_temp_file("test_file_009.txt");
    file f = open(temp_file, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);

    ftime file_time = time(f);
    EXPECT_GT(file_time.create_time, 0);
    EXPECT_GT(file_time.modify_time, 0);
    EXPECT_GT(file_time.access_time, 0);
    close(f);
}

// Tests for set_time
TEST(FileOperationsTest, SetTime) {
    path temp_file = create_temp_file("test_file_010.txt");
    file f = open(temp_file, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);

    ftime new_time{ 1622559600, 1622559600, 1622559600, 1622559600 };
    set_time(f, new_time);

    ftime file_time = time(f);
    EXPECT_EQ(file_time.create_time, new_time.create_time);
    EXPECT_EQ(file_time.modify_time, new_time.modify_time);
    EXPECT_EQ(file_time.access_time, new_time.access_time);
    close(f);
}

// Tests for is_writable
TEST(FileOperationsTest, IsWritable) {
    path temp_file = create_temp_file("test_file_012.txt");
    EXPECT_TRUE(is_writable(temp_file));
}

TEST(FileOperationsTest, IsNotWritable) {
    std::error_code ec;
    bool writable = is_writable("/nonexistent/path/file.txt", ec);
    EXPECT_FALSE(writable);
    EXPECT_TRUE(ec);
}
