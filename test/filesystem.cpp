#include <gtest/gtest.h>
#include <filesystem/file_util.hpp>
#include <filesystem/path_util.hpp>

static const char exist_driver[]			    = R"(C:\)";
static const char exist_file[]					= R"(C:\Windows\notepad.exe)";
static const char exist_directory[]				= R"(C:\Windows)";
static const char current_directory_temp_file[] = "hello.tmp";

TEST(file_util, file_exist)
{
    util::ferror ferr;
    EXPECT_TRUE (util::file_exist(exist_driver, ferr));
    EXPECT_FALSE(ferr);

    EXPECT_TRUE (util::file_exist(exist_file, ferr));
    EXPECT_FALSE(ferr);

    EXPECT_TRUE (util::file_exist(exist_directory, ferr));
    EXPECT_FALSE(ferr);
	
    EXPECT_FALSE(util::file_exist("#$%^^&()", ferr));
    EXPECT_FALSE(ferr);
}

// file_open 只读模式
TEST(file_util, file_open_reonly_and_creat)
{
    util::ferror ferr;
    util::ffile  file;

	// 文件存在则将其删除
    if (util::file_exist(current_directory_temp_file, ferr))
        util::file_remove(current_directory_temp_file, ferr);

    // 以只读打开不存在的文件
    file = util::file_open(current_directory_temp_file, O_RDONLY, ferr);
    EXPECT_TRUE (ferr) << "通过O_RDONLY打开不存在的文件";
    EXPECT_FALSE(file.vaild());

	// 以只读且创建打开不存在的文件
    file = util::file_open(current_directory_temp_file, O_RDONLY | O_CREAT, ferr);
    EXPECT_FALSE(ferr) << "通过O_RDONLY | O_CREAT打开不存在的文件";
    EXPECT_TRUE (file.vaild());

	// 关闭文件
    util::file_close(file);
    EXPECT_FALSE(file.vaild());
    EXPECT_EQ   (0, file.flags());

	// 文件存在
    EXPECT_TRUE (util::file_exist(current_directory_temp_file, ferr));
    EXPECT_FALSE(ferr);

	// 以只读打开存在的文件
    file = util::file_open(current_directory_temp_file, O_RDONLY, ferr);
    EXPECT_FALSE(ferr) << "通过O_RDONLY打开已经存在的文件";
    EXPECT_TRUE (file.vaild());

	// 尝试写只读文件
    std::string wbuff = "Hello World!";
    util::file_write(file, wbuff.data(), wbuff.size(), ferr);
    EXPECT_TRUE (ferr) << "写入只读文件";

    // 文件大小为0
    EXPECT_EQ(0, util::file_size(file, ferr));
    EXPECT_FALSE(ferr);

    // 尝试读只读文件
    std::string rbuff(10, 0);
    util::file_read(file, &rbuff[0], rbuff.size(), ferr);
    EXPECT_TRUE  (ferr) << "读取只读文件"; // 文件读取成功, 但是读取内容太短
    EXPECT_EQ    (0, strlen(rbuff.c_str()));
}

// file_open 只写模式
TEST(file_util, file_open_wronly_and_creat)
{
    util::ferror ferr;
    util::ffile  file;

    // 文件存在则将其删除
    if (util::file_exist(current_directory_temp_file, ferr))
        util::file_remove(current_directory_temp_file, ferr);

    // 以只写打开不存在的文件
    file = util::file_open(current_directory_temp_file, O_WRONLY, ferr);
    EXPECT_TRUE(ferr) << "通过O_WRONLY打开不存在的文件";
    EXPECT_FALSE(file.vaild());

    // 以创建打开不存在的文件
    file = util::file_open(current_directory_temp_file, O_WRONLY | O_CREAT, ferr);
    EXPECT_FALSE(ferr) << "通过O_WRONLY | O_CREAT打开不存在的文件";
    EXPECT_TRUE(file.vaild());

	// 关闭文件
    util::file_close(file);
    EXPECT_FALSE(file.vaild());
    EXPECT_EQ   (0, file.flags());

	// 文件存在
    EXPECT_TRUE (util::file_exist(current_directory_temp_file, ferr));
    EXPECT_FALSE(ferr);

	// 打开存在的文件
    file = util::file_open(current_directory_temp_file, O_WRONLY, ferr);
    EXPECT_FALSE(ferr) << "通过O_WRONLY打开已经存在的文件";
    EXPECT_TRUE (file.vaild());

	// 尝试写文件
    std::string wbuff = "Hello World!";
    util::file_write(file, wbuff.data(), wbuff.size(), ferr);
    EXPECT_FALSE(ferr) << "写入只写文件";

    // 文件大小为0
    EXPECT_EQ	(wbuff.size(), util::file_size(file, ferr));
    EXPECT_FALSE(ferr);

    // 尝试读文件
    std::string rbuff(wbuff.size(), 0);
    util::file_read(file, &rbuff[0], rbuff.size(), ferr);
    EXPECT_TRUE (ferr) << "读取只读文件";
    EXPECT_EQ   (0, strlen(rbuff.c_str()));
}

// file_open 读写模式
TEST(file_util, file_open_rdwr_and_creat)
{
    util::ferror ferr;
    util::ffile  file;

	// 文件存在则将其删除
    if (util::file_exist(current_directory_temp_file, ferr))
        util::file_remove(current_directory_temp_file, ferr);

    // 以打开不存在的文件
    file = util::file_open(current_directory_temp_file, O_RDWR, ferr);
    EXPECT_TRUE (ferr) << "通过O_RDWR打开不存在的文件";
    EXPECT_FALSE(file.vaild());

	// 以创建打开不存在的文件
    file = util::file_open(current_directory_temp_file, O_RDWR | O_CREAT, ferr);
    EXPECT_FALSE(ferr) << "通过O_RDWR | O_CREAT打开不存在的文件";
    EXPECT_TRUE (file.vaild());

	// 关闭文件
    util::file_close(file);
    EXPECT_FALSE(file.vaild());
    EXPECT_EQ   (0, file.flags());

	// 文件存在
    EXPECT_TRUE (util::file_exist(current_directory_temp_file, ferr));
    EXPECT_FALSE(ferr);

	// 打开存在的文件
    file = util::file_open(current_directory_temp_file, O_RDWR, ferr);
    EXPECT_FALSE(ferr) << "通过O_RDWR打开已经存在的文件";
    EXPECT_TRUE (file.vaild());

	// 尝试写文件
    std::string wbuff = "Hello World!";
    util::file_write(file, wbuff.data(), wbuff.size(), ferr);
    EXPECT_FALSE(ferr) << "尝试写文件";

    // 文件大小为0
    EXPECT_EQ(wbuff.size(), util::file_size(file, ferr));
    EXPECT_FALSE(ferr);
    
    // 设置文件指针
    util::file_seek(file, 0, SEEK_SET, ferr);
    EXPECT_FALSE(ferr);
    EXPECT_EQ   (0, util::file_tell(file, ferr));
    EXPECT_FALSE(ferr);

    // 尝试读文件
    std::string rbuff(wbuff.size(), 0);
    util::file_read(file, &rbuff[0], rbuff.size(), ferr);
    EXPECT_FALSE (ferr) << "读取文件";
    EXPECT_EQ    (wbuff.size(), strlen(rbuff.c_str()));
}

// file_open 截断模式
TEST(file_util, file_open_trunc_and_creat)
{
    util::ferror ferr;
    util::ffile  file;

    // 文件存在则将其删除
    if (util::file_exist(current_directory_temp_file, ferr))
        util::file_remove(current_directory_temp_file, ferr);

    // 截断模式打开不存在的文件
    file = util::file_open(current_directory_temp_file, O_RDONLY | O_TRUNC, ferr);
    EXPECT_TRUE(ferr) << "通过O_RDONLY | O_TRUNC打开不存在的文件";
    EXPECT_FALSE(file.vaild());

    // 截断模式打开不存在的文件
    file = util::file_open(current_directory_temp_file, O_WRONLY | O_TRUNC, ferr);
    EXPECT_TRUE (ferr) << "通过O_WRONLY | O_TRUNC打开不存在的文件";
    EXPECT_FALSE(file.vaild());

    // 截断创建模式打开不存在的文件
    file = util::file_open(current_directory_temp_file, O_WRONLY | O_CREAT | O_TRUNC, ferr);
    EXPECT_FALSE(ferr) << "通过O_WRONLY | O_CREAT | O_TRUNC打开不存在的文件";
    EXPECT_TRUE (file.vaild());

    // 尝试写文件
    std::string wbuff = "Hello World!";
    for (int i = 0; i < 10000; ++i)
    {
        util::file_write(file, wbuff.data(), wbuff.size(), ferr);
        EXPECT_FALSE(ferr) << "写入只写文件";
    }

    // 检查文件大小
    EXPECT_EQ	(wbuff.size() * 10000, util::file_size(file, ferr));
    EXPECT_FALSE(ferr);

    // 关闭文件
    file.close();
    EXPECT_FALSE(file.vaild());
    EXPECT_EQ   (0, file.flags());

    // 截断模式打开
    file = util::file_open(current_directory_temp_file, O_WRONLY | O_TRUNC, ferr);
    EXPECT_FALSE(ferr) << "通过O_WRONLY | O_TRUNC打开已经存在的文件";
    EXPECT_TRUE (file.vaild());

    // 检查文件大小
    EXPECT_EQ	(0, util::file_size(file, ferr));
    EXPECT_FALSE(ferr);
}