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

// file_open ֻ��ģʽ
TEST(file_util, file_open_reonly_and_creat)
{
    util::ferror ferr;
    util::ffile  file;

	// �ļ���������ɾ��
    if (util::file_exist(current_directory_temp_file, ferr))
        util::file_remove(current_directory_temp_file, ferr);

    // ��ֻ���򿪲����ڵ��ļ�
    file = util::file_open(current_directory_temp_file, O_RDONLY, ferr);
    EXPECT_TRUE (ferr) << "ͨ��O_RDONLY�򿪲����ڵ��ļ�";
    EXPECT_FALSE(file.vaild());

	// ��ֻ���Ҵ����򿪲����ڵ��ļ�
    file = util::file_open(current_directory_temp_file, O_RDONLY | O_CREAT, ferr);
    EXPECT_FALSE(ferr) << "ͨ��O_RDONLY | O_CREAT�򿪲����ڵ��ļ�";
    EXPECT_TRUE (file.vaild());

	// �ر��ļ�
    util::file_close(file);
    EXPECT_FALSE(file.vaild());
    EXPECT_EQ   (0, file.flags());

	// �ļ�����
    EXPECT_TRUE (util::file_exist(current_directory_temp_file, ferr));
    EXPECT_FALSE(ferr);

	// ��ֻ���򿪴��ڵ��ļ�
    file = util::file_open(current_directory_temp_file, O_RDONLY, ferr);
    EXPECT_FALSE(ferr) << "ͨ��O_RDONLY���Ѿ����ڵ��ļ�";
    EXPECT_TRUE (file.vaild());

	// ����дֻ���ļ�
    std::string wbuff = "Hello World!";
    util::file_write(file, wbuff.data(), wbuff.size(), ferr);
    EXPECT_TRUE (ferr) << "д��ֻ���ļ�";

    // �ļ���СΪ0
    EXPECT_EQ(0, util::file_size(file, ferr));
    EXPECT_FALSE(ferr);

    // ���Զ�ֻ���ļ�
    std::string rbuff(10, 0);
    util::file_read(file, &rbuff[0], rbuff.size(), ferr);
    EXPECT_TRUE  (ferr) << "��ȡֻ���ļ�"; // �ļ���ȡ�ɹ�, ���Ƕ�ȡ����̫��
    EXPECT_EQ    (0, strlen(rbuff.c_str()));
}

// file_open ֻдģʽ
TEST(file_util, file_open_wronly_and_creat)
{
    util::ferror ferr;
    util::ffile  file;

    // �ļ���������ɾ��
    if (util::file_exist(current_directory_temp_file, ferr))
        util::file_remove(current_directory_temp_file, ferr);

    // ��ֻд�򿪲����ڵ��ļ�
    file = util::file_open(current_directory_temp_file, O_WRONLY, ferr);
    EXPECT_TRUE(ferr) << "ͨ��O_WRONLY�򿪲����ڵ��ļ�";
    EXPECT_FALSE(file.vaild());

    // �Դ����򿪲����ڵ��ļ�
    file = util::file_open(current_directory_temp_file, O_WRONLY | O_CREAT, ferr);
    EXPECT_FALSE(ferr) << "ͨ��O_WRONLY | O_CREAT�򿪲����ڵ��ļ�";
    EXPECT_TRUE(file.vaild());

	// �ر��ļ�
    util::file_close(file);
    EXPECT_FALSE(file.vaild());
    EXPECT_EQ   (0, file.flags());

	// �ļ�����
    EXPECT_TRUE (util::file_exist(current_directory_temp_file, ferr));
    EXPECT_FALSE(ferr);

	// �򿪴��ڵ��ļ�
    file = util::file_open(current_directory_temp_file, O_WRONLY, ferr);
    EXPECT_FALSE(ferr) << "ͨ��O_WRONLY���Ѿ����ڵ��ļ�";
    EXPECT_TRUE (file.vaild());

	// ����д�ļ�
    std::string wbuff = "Hello World!";
    util::file_write(file, wbuff.data(), wbuff.size(), ferr);
    EXPECT_FALSE(ferr) << "д��ֻд�ļ�";

    // �ļ���СΪ0
    EXPECT_EQ	(wbuff.size(), util::file_size(file, ferr));
    EXPECT_FALSE(ferr);

    // ���Զ��ļ�
    std::string rbuff(wbuff.size(), 0);
    util::file_read(file, &rbuff[0], rbuff.size(), ferr);
    EXPECT_TRUE (ferr) << "��ȡֻ���ļ�";
    EXPECT_EQ   (0, strlen(rbuff.c_str()));
}

// file_open ��дģʽ
TEST(file_util, file_open_rdwr_and_creat)
{
    util::ferror ferr;
    util::ffile  file;

	// �ļ���������ɾ��
    if (util::file_exist(current_directory_temp_file, ferr))
        util::file_remove(current_directory_temp_file, ferr);

    // �Դ򿪲����ڵ��ļ�
    file = util::file_open(current_directory_temp_file, O_RDWR, ferr);
    EXPECT_TRUE (ferr) << "ͨ��O_RDWR�򿪲����ڵ��ļ�";
    EXPECT_FALSE(file.vaild());

	// �Դ����򿪲����ڵ��ļ�
    file = util::file_open(current_directory_temp_file, O_RDWR | O_CREAT, ferr);
    EXPECT_FALSE(ferr) << "ͨ��O_RDWR | O_CREAT�򿪲����ڵ��ļ�";
    EXPECT_TRUE (file.vaild());

	// �ر��ļ�
    util::file_close(file);
    EXPECT_FALSE(file.vaild());
    EXPECT_EQ   (0, file.flags());

	// �ļ�����
    EXPECT_TRUE (util::file_exist(current_directory_temp_file, ferr));
    EXPECT_FALSE(ferr);

	// �򿪴��ڵ��ļ�
    file = util::file_open(current_directory_temp_file, O_RDWR, ferr);
    EXPECT_FALSE(ferr) << "ͨ��O_RDWR���Ѿ����ڵ��ļ�";
    EXPECT_TRUE (file.vaild());

	// ����д�ļ�
    std::string wbuff = "Hello World!";
    util::file_write(file, wbuff.data(), wbuff.size(), ferr);
    EXPECT_FALSE(ferr) << "����д�ļ�";

    // �ļ���СΪ0
    EXPECT_EQ(wbuff.size(), util::file_size(file, ferr));
    EXPECT_FALSE(ferr);
    
    // �����ļ�ָ��
    util::file_seek(file, 0, SEEK_SET, ferr);
    EXPECT_FALSE(ferr);
    EXPECT_EQ   (0, util::file_tell(file, ferr));
    EXPECT_FALSE(ferr);

    // ���Զ��ļ�
    std::string rbuff(wbuff.size(), 0);
    util::file_read(file, &rbuff[0], rbuff.size(), ferr);
    EXPECT_FALSE (ferr) << "��ȡ�ļ�";
    EXPECT_EQ    (wbuff.size(), strlen(rbuff.c_str()));
}

// file_open �ض�ģʽ
TEST(file_util, file_open_trunc_and_creat)
{
    util::ferror ferr;
    util::ffile  file;

    // �ļ���������ɾ��
    if (util::file_exist(current_directory_temp_file, ferr))
        util::file_remove(current_directory_temp_file, ferr);

    // �ض�ģʽ�򿪲����ڵ��ļ�
    file = util::file_open(current_directory_temp_file, O_RDONLY | O_TRUNC, ferr);
    EXPECT_TRUE(ferr) << "ͨ��O_RDONLY | O_TRUNC�򿪲����ڵ��ļ�";
    EXPECT_FALSE(file.vaild());

    // �ض�ģʽ�򿪲����ڵ��ļ�
    file = util::file_open(current_directory_temp_file, O_WRONLY | O_TRUNC, ferr);
    EXPECT_TRUE (ferr) << "ͨ��O_WRONLY | O_TRUNC�򿪲����ڵ��ļ�";
    EXPECT_FALSE(file.vaild());

    // �ضϴ���ģʽ�򿪲����ڵ��ļ�
    file = util::file_open(current_directory_temp_file, O_WRONLY | O_CREAT | O_TRUNC, ferr);
    EXPECT_FALSE(ferr) << "ͨ��O_WRONLY | O_CREAT | O_TRUNC�򿪲����ڵ��ļ�";
    EXPECT_TRUE (file.vaild());

    // ����д�ļ�
    std::string wbuff = "Hello World!";
    for (int i = 0; i < 10000; ++i)
    {
        util::file_write(file, wbuff.data(), wbuff.size(), ferr);
        EXPECT_FALSE(ferr) << "д��ֻд�ļ�";
    }

    // ����ļ���С
    EXPECT_EQ	(wbuff.size() * 10000, util::file_size(file, ferr));
    EXPECT_FALSE(ferr);

    // �ر��ļ�
    file.close();
    EXPECT_FALSE(file.vaild());
    EXPECT_EQ   (0, file.flags());

    // �ض�ģʽ��
    file = util::file_open(current_directory_temp_file, O_WRONLY | O_TRUNC, ferr);
    EXPECT_FALSE(ferr) << "ͨ��O_WRONLY | O_TRUNC���Ѿ����ڵ��ļ�";
    EXPECT_TRUE (file.vaild());

    // ����ļ���С
    EXPECT_EQ	(0, util::file_size(file, ferr));
    EXPECT_FALSE(ferr);
}