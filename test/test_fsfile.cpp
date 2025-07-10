#include <fstream>
#include "gtest/gtest.h"
#include "utility/error.h"
#include "utility/string.h"
#include "utility/filesystem.h"

using namespace UTILITY_NAMESPACE;
using namespace UTILITY_NAMESPACE::fs;

// Helper function to create temporary file for testing
path create_temp_file(const path& name) {
    path temp_file = path_from_temp("utility") / name;
    std::ofstream(temp_file).close();
    return temp_file;
}

path get_temp_file(const std::string& name = {}) {
    return path_from_temp("utility") / name;
}

static path testFileName = get_temp_file("test_file.txt");

class FileOperationsTest : public ::testing::Test {
public:
    static void SetUpTestSuite() {
        std::error_code ecode;
        remove_all(get_temp_file(), ecode);
        create_directories(get_temp_file(), ecode);
    }

    static void TearDownTestSuite() {
        std::error_code ecode;
        remove_all(get_temp_file(), ecode);
    }

    void SetUp() {
        std::error_code ecode;
        remove(testFileName, ecode);
    }

    void TearDown() {
        std::error_code ecode;
        remove(testFileName, ecode);
    }
};

// Tests for open
TEST_F(FileOperationsTest, OpenFileSuccess) {
    fptr f = open(testFileName, O_RDWR | O_CREAT, 0664);
    ASSERT_NE(f, nullptr);
    close(f);
}

TEST_F(FileOperationsTest, OpenFileFailure) {
    std::error_code ec;
    fptr f = fs::open("/nonexistent/path/file.txt", O_RDWR | O_CREAT, 0664, ec);
    ASSERT_EQ(f, nullptr);
    ASSERT_TRUE(ec);
}

// Tests for close
TEST_F(FileOperationsTest, CloseFile) {
    fptr f = open(testFileName, O_RDWR | O_CREAT, 0664);
    ASSERT_NE(f, nullptr);
    close(f);
    // No direct way to TEST_F close, but no exceptions should be thrown
}

//////////////////////////////////////////////////////////////////////////

// Test open with O_RDONLY
TEST_F(FileOperationsTest, OpenReadOnly) {
    // Create the file first
    std::ofstream outfile(testFileName);
    outfile << "Test data";
    outfile.close();

    std::error_code ec;
    auto testFile = open(testFileName, O_RDONLY, 0664, ec);
    ASSERT_NE(testFile, nullptr);
    EXPECT_FALSE(ec);

    // Check if the file is readable
    char buffer[10];
    size bytesRead = read(testFile, buffer, sizeof(buffer), ec);
    EXPECT_GT(bytesRead, 0);
    EXPECT_FALSE(ec);

    // Check if the file is not writable
    const char* data = "write test";
    size bytesWritten = write(testFile, data, strlen(data), ec);
    EXPECT_EQ(bytesWritten, 0);
    EXPECT_TRUE(ec);

    close(testFile);
}

// Test open with O_WRONLY and O_CREAT
TEST_F(FileOperationsTest, OpenWriteOnlyCreate) {
    std::error_code ec;
    auto testFile = open(testFileName, O_WRONLY | O_CREAT, 0664, ec);
    ASSERT_NE(testFile, nullptr);
    EXPECT_FALSE(ec);

    EXPECT_TRUE(exists(testFileName, ec));
    EXPECT_FALSE(ec);

    // Check if the file is writable
    const char* data = "write test";
    size bytesWritten = write(testFile, data, strlen(data), ec);
    EXPECT_EQ(bytesWritten, strlen(data));
    EXPECT_FALSE(ec);

    // Check if the file is not readable
    char buffer[1024] = {0};
    size bytesRead = read(testFile, buffer, sizeof(buffer), ec);
    EXPECT_EQ(bytesRead, 0);
    EXPECT_TRUE(ec);

    close(testFile);
}

// Test open with O_RDWR and O_CREAT
TEST_F(FileOperationsTest, OpenReadWriteCreate) {
    std::error_code ec;
    auto testFile = open(testFileName, O_RDWR | O_CREAT, 0664, ec);
    ASSERT_NE(testFile, nullptr);
    EXPECT_FALSE(ec);

    EXPECT_TRUE(exists(testFileName, ec));
    EXPECT_FALSE(ec);

    // Check if the file is writable
    const char* data = "write test";
    size bytesWritten = write(testFile, data, strlen(data), ec);
    EXPECT_EQ(bytesWritten, strlen(data));
    EXPECT_FALSE(ec);

    // Check if the file is readable
    char buffer[1024] = {0};
    seek(testFile, 0, SEEK_SET, ec); // Reset the file pointer to the beginning
    size bytesRead = read(testFile, buffer, sizeof(buffer), ec);
    EXPECT_EQ(bytesRead, bytesWritten);
    EXPECT_FALSE(ec);

    close(testFile);
}

// Test open with O_APPEND
TEST_F(FileOperationsTest, OpenAppend) {
    // First create the file
    std::error_code ec;
    auto testFile = open(testFileName, O_WRONLY | O_CREAT, 0664, ec);
    ASSERT_NE(testFile, nullptr);
    close(testFile);

    // Open the file in append mode
    testFile = open(testFileName, O_WRONLY | O_APPEND, 0664, ec);
    ASSERT_NE(testFile, nullptr);
    EXPECT_FALSE(ec);

    // Check if the file is writable
    const char* data = "append test";
    size bytesWritten = write(testFile, data, strlen(data), ec);
    EXPECT_EQ(bytesWritten, strlen(data));
    EXPECT_FALSE(ec);

    close(testFile);
}

// Test open with O_TRUNC
TEST_F(FileOperationsTest, OpenTruncate) {
    // First create the file with some content
    std::error_code ec;
    auto testFile = open(testFileName, O_WRONLY | O_CREAT, 0664, ec);
    ASSERT_NE(testFile, nullptr);
    const char* data = "Initial content";
    write(testFile, data, strlen(data));
    close(testFile);

    // Open the file in truncate mode
    testFile = open(testFileName, O_WRONLY | O_TRUNC, 0664, ec);
    ASSERT_NE(testFile, nullptr);
    EXPECT_FALSE(ec);

    auto bytes = file_size(testFile, ec);
    EXPECT_EQ(bytes, 0);
    EXPECT_FALSE(ec);
    close(testFile);

    // Verify the file is truncated
    char buffer[20];
    testFile = open(testFileName, O_RDONLY, 0664, ec);
    size bytesRead = read(testFile, buffer, sizeof(buffer), ec);
    EXPECT_EQ(bytesRead, 0);
    EXPECT_FALSE(ec);

    close(testFile);
}

// Test open with O_EXCL
TEST_F(FileOperationsTest, OpenExclusive) {
    // First create the file
    std::error_code ec;
    auto testFile = open(testFileName, O_WRONLY | O_CREAT, 0664, ec);
    ASSERT_NE(testFile, nullptr);
    EXPECT_FALSE(ec);
    close(testFile);

    // Try to open the file exclusively
    testFile = open(testFileName, O_WRONLY | O_CREAT | O_EXCL, 0664, ec);
    EXPECT_EQ(testFile, nullptr);
    EXPECT_TRUE(ec);  // Expect file exists error
}

// Test open with O_CREAT | O_EXCL for non-existent file
TEST_F(FileOperationsTest, OpenCreateExclusiveNonExistent) {
    std::error_code ec;
    auto testFile = open(testFileName, O_WRONLY | O_CREAT | O_EXCL, 0664, ec);
    ASSERT_NE(testFile, nullptr);
    EXPECT_FALSE(ec);

    EXPECT_TRUE(exists(testFileName, ec));
    EXPECT_FALSE(ec);

    close(testFile);
}

//////////////////////////////////////////////////////////////////////////

// Tests for read
TEST_F(FileOperationsTest, ReadFile) {
    fptr f = open(testFileName, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);
    const char* write_data = "TEST_F data";
    int len = strlen(write_data);
    int bytes_write = write(f, write_data, len);

    EXPECT_EQ(len, bytes_write);
    seek(f, 0, SEEK_SET);

    char read_data[1024] = { 0 };
    size bytes_read = read(f, read_data, len);
    EXPECT_EQ(bytes_read, len);
    EXPECT_EQ(std::string(read_data), "TEST_F data");
    close(f);
}

TEST_F(FileOperationsTest, ReadFileError) {
    fptr f = open(testFileName, O_RDWR | O_CREAT);
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
    fptr f = open(testFileName, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);

    const char* write_data = "TEST_F data";
    int len = strlen(write_data);
    size bytes_written = write(f, write_data, len);
    EXPECT_EQ(bytes_written, len);

    seek(f, 0, SEEK_SET);
    char read_data[1024] = { 0 };
    size bytes_read = read(f, read_data, len);
    EXPECT_EQ(std::string(read_data), write_data);
    EXPECT_EQ(bytes_read, bytes_written);
    close(f);
}

// Tests for seek and tell
TEST_F(FileOperationsTest, SeekAndTell) {
    fptr f = open(testFileName, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);

    const char* write_data = "TEST_F data";
    int len = strlen(write_data);
    size bytes_written = write(f, write_data, len);
    EXPECT_EQ(bytes_written, len);
    EXPECT_EQ(tell(f), bytes_written);

    size fileSize = bytes_written;

    seek(f, 7, SEEK_SET);
    EXPECT_EQ(tell(f), 7);

    char read_data[1024] = { 0 };
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
    fptr f = open(testFileName, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);

    const char* write_data = "TEST_F data";
    write(f, write_data, strlen(write_data));

    EXPECT_EQ(file_size(f), strlen(write_data));
    close(f);
}

// Tests for time
TEST_F(FileOperationsTest, FileTime) {
    fptr f = open(testFileName, O_RDWR | O_CREAT);
    ASSERT_NE(f, nullptr);

    ftime file_time = time(f);
    EXPECT_GT(file_time.create_time, 0);
    EXPECT_GT(file_time.modify_time, 0);
    EXPECT_GT(file_time.access_time, 0);
    close(f);
}

// Tests for set_time
TEST_F(FileOperationsTest, SetTime) {
    fptr f = open(testFileName, O_RDWR | O_CREAT);
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
    path temp_file = create_temp_file(testFileName.filename());
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
