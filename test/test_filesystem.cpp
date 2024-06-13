#include <fstream>
#include <gtest/gtest.h>
#include "utility/error.h"
#include "utility/string.h"
#include "utility/filesystem.h"

using namespace util;
using namespace util::fs;

class FileOperationsTest : public ::testing::Test {
public:
    static void SetUpTestSuite()
    {
        std::error_code ecode;
        remove_all(path_from_temp("utility"), ecode);
        create_directories(path_from_temp("utility"), ecode);
    }

    static void TearDownTestSuite()
    {
        std::error_code ecode;
        remove_all(path_from_temp("utility"), ecode);
    }
};

// Helper function to create temporary file for testing
path create_temp_file(const std::string& name) {
    path temp_file = path_from_temp("utility") / name;
    std::ofstream(temp_file).close();
    return temp_file;
}

path get_temp_file(const std::string& name) {
    return path_from_temp("utility") / name;
}

// Tests for open
TEST_F(FileOperationsTest, OpenFileSuccess) {
    path temp_file = create_temp_file("test_file_002.txt");
    file f = open(temp_file, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);
    close(f);
}

TEST_F(FileOperationsTest, OpenFileFailure) {
    std::error_code ec;
    file f = fs::open("/nonexistent/path/file.txt", O_RDWR | O_CREAT, ec);
    ASSERT_EQ(f, nullptr);
    ASSERT_TRUE(ec);
}

// Tests for close
TEST_F(FileOperationsTest, CloseFile) {
    path temp_file = create_temp_file("test_file_003.txt");
    file f = open(temp_file, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);
    close(f);
    // No direct way to TEST_F close, but no exceptions should be thrown
}

// Tests for read
TEST_F(FileOperationsTest, ReadFile) {
    path temp_file = get_temp_file("test_file_004.txt");
    file f = open(temp_file, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);
    const char* write_data = "TEST_F data";
    int len = strlen(write_data);
    int bytes_write = write(f, write_data, len);

    EXPECT_EQ(len, bytes_write);
    seek(f, 0, SEEK_SET);

    char read_data[1024] = {0};
    size bytes_read = read(f, read_data, len);
    EXPECT_EQ(bytes_read, len);
    EXPECT_EQ(std::string(read_data), "TEST_F data");
    close(f);
}

TEST_F(FileOperationsTest, ReadFileError) {
    path temp_file = get_temp_file("test_file_005.txt");
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
TEST_F(FileOperationsTest, WriteFile) {
    path temp_file = get_temp_file("test_file_006.txt");
    file f = open(temp_file, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);

    const char* write_data = "TEST_F data";
    int len = strlen(write_data);
    size bytes_written = write(f, write_data, len);
    EXPECT_EQ(bytes_written, len);

    seek(f, 0, SEEK_SET);
    char read_data[1024] = {0};
    size bytes_read = read(f, read_data, len);
    EXPECT_EQ(std::string(read_data), write_data);
    EXPECT_EQ(bytes_read, bytes_written);
    close(f);
}

// Tests for seek and tell
TEST_F(FileOperationsTest, SeekAndTell) {
    path temp_file = get_temp_file("test_file_007.txt");
    file f = open(temp_file, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);

    const char* write_data = "TEST_F data";
    int len = strlen(write_data);
    size bytes_written = write(f, write_data, len);
    EXPECT_EQ(bytes_written, len);
    EXPECT_EQ(tell(f), bytes_written);

    size fileSize = bytes_written;

    seek(f, 7, SEEK_SET);
    EXPECT_EQ(tell(f), 7);

    char read_data[1024] = {0};
    size bytes_read = read(f, read_data, 10);
    EXPECT_EQ(std::string(read_data), "data");

    seek(f, 0, SEEK_CUR);
    EXPECT_EQ(tell(f), fileSize);

    seek(f, 0, SEEK_SET);
    EXPECT_EQ(tell(f), 0);

    seek(f, 4, SEEK_SET);
    EXPECT_EQ(tell(f), 4);

    seek(f, 1, SEEK_CUR);
    EXPECT_EQ(tell(f), 5);

    seek(f, 15, SEEK_CUR);
    EXPECT_EQ(tell(f), 20);

    seek(f, 7, SEEK_SET);
    EXPECT_EQ(tell(f), 7);

    seek(f, 0, SEEK_END);
    EXPECT_EQ(tell(f), fileSize);

    close(f);
}

// Tests for file_size
TEST_F(FileOperationsTest, FileSize) {
    path temp_file = get_temp_file("test_file_008.txt");
    file f = open(temp_file, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);

    const char* write_data = "TEST_F data";
    write(f, write_data, strlen(write_data));

    EXPECT_EQ(file_size(f), strlen(write_data));
    close(f);
}

// Tests for time
TEST_F(FileOperationsTest, FileTime) {
    path temp_file = get_temp_file("test_file_009.txt");
    file f = open(temp_file, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);

    ftime file_time = time(f);
    EXPECT_GT(file_time.create_time, 0);
    EXPECT_GT(file_time.modify_time, 0);
    EXPECT_GT(file_time.access_time, 0);
    close(f);
}

// Tests for set_time
TEST_F(FileOperationsTest, SetTime) {
    path temp_file = get_temp_file("test_file_010.txt");
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
TEST_F(FileOperationsTest, IsWritable) {
    std::error_code ec;
    path temp_file = create_temp_file("test_file_012.txt");
    auto writable = is_writable(temp_file, ec);
    EXPECT_TRUE(writable);
    EXPECT_FALSE(ec);
}

TEST_F(FileOperationsTest, IsNotWritable) {
    std::error_code ec;
    bool writable = is_writable("/nonexistent/path/file.txt", ec);
    EXPECT_FALSE(writable);
    EXPECT_TRUE(ec);
}
