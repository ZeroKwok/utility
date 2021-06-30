#include <gtest/gtest.h>
#include <common/bytedata.hpp>

TEST(common_bytedata, bytes_base64)
{
    std::string srcByte = 
        R"(对于 Windows 操作系统而言，要想成功的删除一个文件或重命名一个文件，需要满足一个条件：文件不能被占用。)";
    std::string destByte;

    util::bytes_into_base64(destByte, srcByte);
    EXPECT_FALSE(destByte.empty());

    util::bytedata bytes;
    util::bytes_from_base64(bytes, destByte);
    EXPECT_EQ(bytes, srcByte);
}

TEST(common_bytedata, bytes_hex)
{
    std::string srcByte =
        R"(对于 Windows 操作系统而言，要想成功的删除一个文件或重命名一个文件，需要满足一个条件：文件不能被占用。)";

    std::string destByte= util::bytes_into_hex(srcByte);
    EXPECT_FALSE(destByte.empty());

    util::bytedata bytes = util::bytes_from_hex(destByte);
    EXPECT_EQ(bytes, srcByte);

    destByte = util::bytes_into_hex(srcByte, " ", 2);
    EXPECT_FALSE(destByte.empty());

    bytes = util::bytes_from_hex(destByte);
    EXPECT_EQ(bytes, srcByte);

    destByte = util::bytes_into_hex(srcByte, "-", 4);
    EXPECT_FALSE(destByte.empty());

    bytes = util::bytes_from_hex(destByte);
    EXPECT_EQ(bytes, srcByte);
}

TEST(common_bytedata, bytes_from)
{
    struct person
    {
        int    age;
        int    sex;
        bool   adult;
        double height;
        char   name[30];
    };

    person lining  = { 32, 1, true, 165.36, "李宁" };
    
    util::bytedata bytes = util::bytes_from(lining);
    EXPECT_FALSE(bytes.empty());

    person lining1 = util::bytes_cast<person>(bytes);
    EXPECT_TRUE(lining1.age == lining.age);
    EXPECT_TRUE(lining1.sex == lining.sex);
    EXPECT_TRUE(lining1.adult == lining.adult);
    EXPECT_TRUE(lining1.height == lining.height);
    EXPECT_TRUE(strcmp(lining1.name, lining.name) == 0);

    lining1 = util::bytes_cast<person>(bytes, 0);
    EXPECT_TRUE(lining1.age == lining.age);
    EXPECT_TRUE(lining1.sex == lining.sex);
    EXPECT_TRUE(lining1.adult == lining.adult);
    EXPECT_TRUE(lining1.height == lining.height);
    EXPECT_TRUE(strcmp(lining1.name, lining.name) == 0);

    util::bytedata bytes1 = util::bytes_from_rang(&lining.age, &lining.name);

    // 因为字节对齐. 可能会在name后面填充字节, 30 + 2才是4的整倍数
    bytes1.resize(sizeof person);

    lining1 = util::bytes_cast<person>(bytes1);
    EXPECT_TRUE(lining1.age == lining.age);
    EXPECT_TRUE(lining1.sex == lining.sex);
    EXPECT_TRUE(lining1.adult == lining.adult);
    EXPECT_TRUE(lining1.height == lining.height);
    EXPECT_TRUE(strcmp(lining1.name, lining.name) == 0);
}