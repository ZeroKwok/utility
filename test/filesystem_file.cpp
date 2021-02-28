#include <gtest/gtest.h>
#include <filesystem/file_util.h>
#include <filesystem/path_util.h>

#if OS_POSIX
static const char exist_root[]                  = R"(/)";
static const char exist_directory[]             = R"(/etc)";
static const char exist_file[]                  = R"(/etc/passwd)";
#else
static const char exist_root[]                  = R"(C:\)";
static const char exist_directory[]             = R"(C:\Windows)";
static const char exist_file[]                  = R"(C:\Windows\notepad.exe)";
#endif

static const char  current_directory_temp_file [] = "hello1.tmp";
static const char  current_directory_temp_file2[] = "hello2.tmp";

static std::string gloabl_buffer                  = "Hello Utility!";

TEST(file_util, file_exist)
{
    util::ferror ferr;
    EXPECT_TRUE (util::file_exist(exist_root, ferr));
    EXPECT_FALSE(ferr);

    EXPECT_TRUE (util::file_exist(exist_file, ferr));
    EXPECT_FALSE(ferr);

    EXPECT_TRUE (util::file_exist(exist_directory, ferr));
    EXPECT_FALSE(ferr);
    
    EXPECT_FALSE(util::file_exist("#$%^^&()", ferr));
    EXPECT_FALSE(ferr);
}

TEST(file_util, file_remove)
{
    util::ferror ferr;

    // 文件存在则将其删除
    if (util::file_exist(current_directory_temp_file, ferr))
        util::file_remove(current_directory_temp_file, ferr);

    // 创建文件
    {
        util::file_open(current_directory_temp_file, O_CREAT, ferr);
    }

    // 移除文件
    util::file_remove(current_directory_temp_file, ferr);
    EXPECT_FALSE(ferr);

    auto exist = util::file_exist(current_directory_temp_file, ferr);
    EXPECT_FALSE(exist);
    EXPECT_FALSE(ferr);
}

TEST(file_util, file_copy)
{
    util::ferror ferr;

    if (util::file_exist(current_directory_temp_file, ferr))
        util::file_remove(current_directory_temp_file, ferr);

    if (util::file_exist(current_directory_temp_file2, ferr))
        util::file_remove(current_directory_temp_file2, ferr);

    {
        util::ffile file = util::file_open(current_directory_temp_file, O_CREAT | O_WRONLY, ferr);

        // 制造空洞文件 或者 sparse file（稀疏文件）
        util::file_write(file, gloabl_buffer.data(), gloabl_buffer.size());
        util::file_seek(file, 5000, SEEK_SET);
        util::file_write(file, gloabl_buffer.data(), gloabl_buffer.size());
    }

    // 拷贝文件
    util::file_copy(current_directory_temp_file, current_directory_temp_file2, ferr);
    EXPECT_FALSE(ferr);

    // 检查文件是否存在
    auto exist = util::file_exist(current_directory_temp_file2, ferr);
    EXPECT_TRUE(exist);
    EXPECT_FALSE(ferr);

    auto size1 = util::file_size(current_directory_temp_file, ferr);
    EXPECT_FALSE(ferr);

    auto size2 = util::file_size(current_directory_temp_file2, ferr);
    EXPECT_FALSE(ferr);

    EXPECT_EQ(size1, size2);

    // 缺少判断空洞文件代码
}

TEST(file_util, file_move)
{
    util::ferror ferr;

    if (util::file_exist(current_directory_temp_file, ferr))
        util::file_remove(current_directory_temp_file, ferr);

    if (util::file_exist(current_directory_temp_file2, ferr))
        util::file_remove(current_directory_temp_file2, ferr);

    {
        util::ffile file = util::file_open(current_directory_temp_file, O_CREAT | O_WRONLY, ferr);

        // 制造空洞文件 或者 sparse file（稀疏文件）
        util::file_write(file, gloabl_buffer.data(), gloabl_buffer.size());
        util::file_seek(file, 5000, SEEK_SET);
        util::file_write(file, gloabl_buffer.data(), gloabl_buffer.size());
    }

    // 通文件系统 移动文件
    util::file_move(current_directory_temp_file, current_directory_temp_file2, ferr);
    EXPECT_FALSE(ferr);

    auto exist1 = util::file_exist(current_directory_temp_file, ferr);
    EXPECT_FALSE(ferr);
    EXPECT_FALSE(exist1);

    auto exist2 = util::file_exist(current_directory_temp_file2, ferr);
    EXPECT_FALSE(ferr);
    EXPECT_TRUE(exist2);

    // 缺少跨文件系统的移动
}

TEST(file_util, file_time_and_set)
{
    util::ferror ferr;
    util::ffile  file;

    if (!util::file_exist(current_directory_temp_file, ferr))
        util::file_open(current_directory_temp_file, O_CREAT, ferr);

    util::ftime ft = util::file_time(current_directory_temp_file, ferr);
    EXPECT_FALSE(ferr);

    EXPECT_NE(ft.modify_time, -1);
    EXPECT_NE(ft.access_time, -1);

#if OS_WIN
    EXPECT_NE(ft.create_time, -1);
    EXPECT_EQ(ft.change_time, -1);
#else
    EXPECT_EQ(ft.create_time, -1);
    EXPECT_NE(ft.change_time, -1);
#endif

    util::ftime ft1 = { -1, -1, -1, -1 };
    ft1.access_time = time(0);
    ft1.modify_time = time(0) + 100;

#if OS_WIN
    ft1.create_time = time(0) + 150;
#endif

    util::file_set_time(current_directory_temp_file, ft1, ferr);
    EXPECT_FALSE(ferr);

    ft = util::file_time(current_directory_temp_file, ferr);
    EXPECT_FALSE(ferr);

    EXPECT_EQ(ft.access_time, ft1.access_time);
    EXPECT_EQ(ft.modify_time, ft1.modify_time);

#if OS_WIN
    EXPECT_EQ(ft.create_time, ft1.create_time);
    EXPECT_EQ(ft.change_time, -1);
#else
    EXPECT_EQ(ft.create_time, -1);
    EXPECT_NE(ft.change_time, -1);
#endif
    // 缺以文件描述符的时间
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
    EXPECT_EQ    (wbuff.size(), util::file_size(file, ferr));
    EXPECT_FALSE(ferr);

    // 尝试读文件
    std::string rbuff(wbuff.size(), 0);
    util::file_read(file, &rbuff[0], rbuff.size(), ferr);
    EXPECT_TRUE (ferr) << "读取只写文件";
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
    EXPECT_EQ    (wbuff.size() * 10000, util::file_size(file, ferr));
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
    EXPECT_EQ    (0, util::file_size(file, ferr));
    EXPECT_FALSE(ferr);
}

TEST(file_util, file_is_writable)
{
    util::ferror ferr;
    util::ffile  file;

    if (!util::file_exist(current_directory_temp_file, ferr))
        util::file_open(current_directory_temp_file, O_CREAT, ferr);

    auto wriable1 = util::file_is_writable(current_directory_temp_file, ferr);
    EXPECT_FALSE(ferr);
    EXPECT_TRUE(wriable1);

    auto wriable2 = util::file_is_writable(exist_file, ferr);
    EXPECT_FALSE(ferr);
    EXPECT_FALSE(wriable2);
}

TEST(file_util, directories_create_and_remove)
{
    util::ferror ferr;
    util::directories_create("./parent/child/grandson", ferr);
    EXPECT_FALSE(ferr);

    auto exist1 = util::file_exist("./parent/child/grandson", ferr);
    EXPECT_TRUE(exist1);
    EXPECT_FALSE(ferr);

    util::file_open("./parent/child/grandson/file", O_CREAT, ferr);
    EXPECT_FALSE(ferr);

    util::directories_remove("./parent", ferr);
    EXPECT_FALSE(ferr);

    exist1 = util::file_exist("./parent", ferr);
    EXPECT_FALSE(exist1);
    EXPECT_FALSE(ferr);
}
































