#include <fstream>
#include "gtest/gtest.h"
#include "utility/error.h"
#include "utility/string.h"
#include "utility/filesystem.h"

using namespace UTILITY_NAMESPACE;
using namespace UTILITY_NAMESPACE::fs;

TEST(ErrorTest, ErrorCategoryBasicProperties)
{
    auto &cat = error_category::instance();

    EXPECT_STREQ("Error", cat.name());
    EXPECT_EQ(std::string("Succeed"), cat.message(kSucceed));
    EXPECT_EQ(std::string("Error: 0x00000001"), cat.message(kInvalidParam));
    EXPECT_EQ(std::string("Error: 0x000000a1"), cat.message(kServerError));
}

TEST(ErrorTest, MakeErrorBasic)
{
    auto ec1 = make_error(kSucceed);
    EXPECT_EQ(kSucceed, ec1.value());
    EXPECT_STREQ("Error", ec1.category().name());
    EXPECT_EQ("Succeed", ec1.message());

    auto ec2 = make_error(kFileNotFound);
    EXPECT_EQ(kFileNotFound, ec2.value());
    EXPECT_EQ("Error: 0x00000041", ec2.message());
}

TEST(ErrorTest, ErrorCodeComparison)
{
    auto ec1 = make_error(kSucceed);
    auto ec2 = make_error(kSucceed);
    auto ec3 = make_error(kInvalidParam);

    EXPECT_EQ(ec1, ec2);
    EXPECT_NE(ec1, ec3);
}

// Test for make_error_from_native would typically require mocking system errors
// Here's a basic test structure - actual implementation would depend on platform specifics
TEST(ErrorTest, MakeErrorFromNativeBasic)
{
    // Test with zero error code (success)
    auto ec1 = make_error_from_native(0);
    EXPECT_EQ(kSucceed, ec1.value());

    // Test with default error code
    auto ec2 = make_error_from_native(9999); // Some non-existent error code
    EXPECT_EQ(kRuntimeError, ec2.value());

    // Note: Platform-specific tests would be needed here to test actual system error mappings
    // For example, on Windows you might test ERROR_FILE_NOT_FOUND mapping to kFileNotFound
}

TEST(ErrorTest, FilesystemErrorMapping)
{
    namespace fs = std::filesystem;

    // Test filesystem error mapping (simulated)
    // Note: Actual implementation would need platform-specific error codes
#if OS_WIN
    int code = ERROR_DISK_FULL, code2 = ERROR_PATH_NOT_FOUND;
#else
    int code = ENOSPC, code2 = ENOENT;
#endif

    auto ec1 = make_error_from_native(code, 
        path_from_module_dir(), kFilesystemError);
    EXPECT_TRUE(ec1.value() == kFilesystemNoSpace || ec1.value() == kFilesystemNotSupportLargeFiles);

    auto ec2 = make_error_from_native(
        code2,
        {},
        kFilesystemError);

    EXPECT_TRUE(ec2.value() == kFileNotFound);
}

TEST(ErrorTest, ErrorCodeUsageWithSTL)
{
    std::error_code ec = make_error(kFileNotFound);

    EXPECT_TRUE(ec);
    EXPECT_EQ(kFileNotFound, ec.value());

    std::error_code success_ec = make_error(kSucceed);
    EXPECT_FALSE(success_ec);
}
