#include <gtest/gtest.h>
#include "utility/string.h"

using namespace util;

// Test cases for utf8_to_wstring function
TEST(StringConversionTest, Utf8ToWstring) {
    // Test converting ASCII UTF-8 string
    std::string asciiInput = (const char*)u8"Test";
    std::wstring asciiOutput;
    utf8_to_wstring(asciiInput, asciiOutput);
    EXPECT_EQ(asciiOutput, L"Test");

    // Test converting Unicode UTF-8 string
    std::string unicodeInput = (const char*)u8"你好";
    std::wstring unicodeOutput;
    utf8_to_wstring(unicodeInput, unicodeOutput);
    EXPECT_EQ(unicodeOutput, L"你好");

    // Test converting empty UTF-8 string
    std::string emptyInput = "";
    std::wstring emptyOutput;
    utf8_to_wstring(emptyInput, emptyOutput);
    EXPECT_EQ(emptyOutput, L"");
}

// Test cases for utf8_to_string function
TEST(StringConversionTest, Utf8ToString) {
    // Test converting ASCII UTF-8 string
    std::string asciiInput = "Test";
    std::string asciiOutput;
    utf8_to_string(asciiInput, asciiOutput);
    EXPECT_EQ(asciiOutput, (const char*)u8"Test");

    // Test converting Unicode UTF-8 string
    std::string unicodeInput = (const char*)u8"你好";
    std::string unicodeOutput;
    utf8_to_string(unicodeInput, unicodeOutput);

    // EXPECT_EQ(unicodeOutput, "你好");
    std::wstring wstringOutput;
    string_to_wstring(unicodeOutput, wstringOutput);
    EXPECT_EQ(wstringOutput, L"你好");

    // Test converting empty UTF-8 string
    std::string emptyInput = "";
    std::string emptyOutput;
    utf8_to_string(emptyInput, emptyOutput);
    EXPECT_EQ(emptyOutput, "");
}

// Test cases for string_to_wstring function
TEST(StringConversionTest, StringToWstring) {
    // Test converting ASCII string
    std::string asciiInput = "Test";
    std::wstring asciiOutput;
    string_to_wstring(asciiInput, asciiOutput);
    EXPECT_EQ(asciiOutput, L"Test");

    // Test converting Unicode string
    std::wstring unicodeOutput;
    utf8_to_string("你好", asciiInput);
    string_to_wstring(asciiInput, unicodeOutput);
    EXPECT_EQ(unicodeOutput, L"你好");

    // Test converting empty string
    std::string emptyInput = "";
    std::wstring emptyOutput;
    string_to_wstring(emptyInput, emptyOutput);
    EXPECT_EQ(emptyOutput, L"");
}

// Test cases for wstring_to_string function
TEST(StringConversionTest, WstringToString) {
    // Test converting ASCII wstring
    std::wstring asciiInput = L"Test";
    std::string asciiOutput;
    wstring_to_string(asciiInput, asciiOutput);
    EXPECT_EQ(asciiOutput, "Test");

    // Test converting Unicode wstring
    std::wstring unicodeInput = L"你好";
    std::string unicodeOutput;
    wstring_to_string(unicodeInput, unicodeOutput);
    utf8_to_string((const char*)u8"你好", asciiOutput);
    EXPECT_EQ(unicodeOutput, asciiOutput);

    // Test converting empty wstring
    std::wstring emptyInput = L"";
    std::string emptyOutput;
    wstring_to_string(emptyInput, emptyOutput);
    EXPECT_EQ(emptyOutput, "");
}

// Test cases for wstring_to_utf8 function
TEST(StringConversionTest, WstringToUtf8) {
    // Test converting ASCII wstring
    std::wstring asciiInput = L"Test";
    std::string asciiOutput;
    wstring_to_utf8(asciiInput, asciiOutput);
    EXPECT_EQ(asciiOutput, (const char*)u8"Test");

    // Test converting Unicode wstring
    std::wstring unicodeInput = L"你好";
    std::string unicodeOutput;
    wstring_to_utf8(unicodeInput, unicodeOutput);
    EXPECT_EQ(unicodeOutput, (const char*)u8"你好");

    // Test converting empty wstring
    std::wstring emptyInput = L"";
    std::string emptyOutput;
    wstring_to_utf8(emptyInput, emptyOutput);
    EXPECT_EQ(emptyOutput, "");
}

// Test cases for string_to_utf8 function
TEST(StringConversionTest, StringToUtf8) {
    // Test converting ASCII string
    std::string asciiInput = "Test";
    std::string asciiOutput;
    string_to_utf8(asciiInput, asciiOutput);
    EXPECT_EQ(asciiOutput, (const char*)u8"Test");

    // Test converting Unicode string
    std::wstring unicodeInput = L"你好";
    std::string unicodeOutput;
    wstring_to_string(unicodeInput, asciiInput);
    string_to_utf8(asciiInput, unicodeOutput);
    EXPECT_EQ(unicodeOutput, (const char*)u8"你好");

    // Test converting empty string
    std::string emptyInput = "";
    std::string emptyOutput;
    string_to_utf8(emptyInput, emptyOutput);
    EXPECT_EQ(emptyOutput, "");
}

// Test cases for str function
TEST(StringConversionTest, Str) {
    // Test converting wstring to string
    std::wstring wstrInput = L"Test";
    std::string output = str(wstrInput);
    EXPECT_EQ(output, "Test");

    // Test converting string to string (no conversion)
    std::string strInput = "Test";
    output = str(strInput);
    EXPECT_EQ(output, "Test");
}

// Test cases for str_u8 function
TEST(StringConversionTest, StrU8) {
    // Test converting UTF-8 string to string
    std::string utf8Input = (const char*)u8"你好";
    std::string output = str_u8(utf8Input);
    std::wstring wstring;
    string_to_wstring(output, wstring);
    EXPECT_EQ(wstring, L"你好");
}

// Test cases for utf8 function
TEST(StringConversionTest, Utf8) {
    // Test converting wstring to UTF-8 string
    std::wstring wstrInput = L"你好";
    std::string output = utf8(wstrInput);
    EXPECT_EQ(output, (const char*)u8"你好");

#if OS_POSIX
    // Test converting UTF-8 string to UTF-8 string (no conversion)
    std::string utf8Input = (const char*)u8"你好";
    output = utf8(utf8Input);
    EXPECT_EQ(output, (const char*)u8"你好");
#endif
}

// Test cases for wstr function
TEST(StringConversionTest, Wstr) {
    // Test converting string to wstring
    std::string strInput = "Test";
    std::wstring output = wstr(strInput);
    EXPECT_EQ(output, L"Test");

    // Test converting wstring to wstring (no conversion)
    std::wstring wstrInput = L"Test";
    output = wstr(wstrInput);
    EXPECT_EQ(output, L"Test");
}

// Test cases for wstr_u8 function
TEST(StringConversionTest, WstrU8) {
    // Test converting UTF-8 string to wstring
    std::string utf8Input = (const char*)u8"你好";
    std::wstring output = wstr_u8(utf8Input);
    EXPECT_EQ(output, L"你好");
}

// Test cases for str function with filesystem path
TEST(StringConversionTest, StrFileSystemPath) {
    // Test converting filesystem path to string
    std::filesystem::path path = "test.txt";
    std::string output = str(path);
    EXPECT_EQ(output, "test.txt");
}

// Test cases for wstr function with filesystem path
TEST(StringConversionTest, WstrFileSystemPath) {
    // Test converting filesystem path to wstring
    std::filesystem::path path = "test.txt";
    std::wstring output = wstr(path);
    EXPECT_EQ(output, L"test.txt");
}

// Test cases for utf8 function with filesystem path
TEST(StringConversionTest, Utf8FileSystemPath) {
    // Test converting filesystem path to UTF-8 string
    std::filesystem::path path = L"你好.txt";
    std::string output = utf8(path);
    EXPECT_EQ(output, (const char*)u8"你好.txt");
}

#ifdef UTILITY_SUPPORT_QT

// Test cases for str function with QString
TEST(StringConversionTest, StrQString) {
    // Test converting QString to string
    QString qstr = "Test";
    std::string output = str(qstr);
    EXPECT_EQ(output, "Test");
}

// Test cases for utf8 function with QString
TEST(StringConversionTest, Utf8QString) {
    // Test converting QString to UTF-8 string
    QString qstr = (const char*)u8"你好";
    std::string output = utf8(qstr);
    EXPECT_EQ(output, (const char*)u8"你好");
}

// Test cases for wstr function with QString
TEST(StringConversionTest, WstrQString) {
    // Test converting QString to wstring
    QString qstr = "Test";
    std::wstring output = wstr(qstr);
    EXPECT_EQ(output, L"Test");
}

// Test cases for qstr function with std::string
TEST(StringConversionTest, QstrStdString) {
    // Test converting std::string to QString
    std::string str = "Test";
    QString output = qstr(str);
    EXPECT_EQ(output, "Test");
}

// Test cases for qstr function with std::wstring
TEST(StringConversionTest, QstrStdWstring) {
    // Test converting std::wstring to QString
    std::wstring wstr = L"你好";
    QString output = qstr(wstr);
    EXPECT_EQ(output, (const char*)u8"你好");
}

// Test cases for qstr_u8 function with std::string
TEST(StringConversionTest, QstrU8StdString) {
    // Test converting UTF-8 string to QString
    std::string utf8str = (const char*)u8"你好";
    QString output = qstr_u8(utf8str);
    EXPECT_EQ(output, (const char*)u8"你好");
}

// Test cases for qstr function with filesystem path
TEST(StringConversionTest, QstrFileSystemPath) {
    // Test converting filesystem path to QString
    std::filesystem::path path = "test.txt";
    QString output = qstr(path);
    EXPECT_EQ(output, "test.txt");
}

#endif // UTILITY_SUPPORT_QT