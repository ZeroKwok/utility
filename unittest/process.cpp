#include <windows.h>
#include <gtest/gtest.h>
#include <process/process.h>
#include <string/string_conv_easy.hpp>

TEST(process, default_constructor)
{
    util::process process;
    util::platform_error error;

    EXPECT_FALSE(process.valid());
    EXPECT_FALSE(process.joinable());
    EXPECT_FALSE(process.running());
    EXPECT_TRUE (process.wait_until(0, error));

    EXPECT_EQ(-1, process.id());
    EXPECT_EQ(-1, process.native_handle());
    EXPECT_EQ(-1, process.exit_code());
}

TEST(process, pid_constructor)
{
    util::platform_error error;
    util::process process(::GetCurrentProcessId(), error);

    EXPECT_TRUE(process.valid());
    EXPECT_TRUE(process.joinable());
    EXPECT_TRUE(process.running());
    EXPECT_FALSE(process.wait_until(0, error));
    EXPECT_TRUE(error);

    EXPECT_EQ(::GetCurrentProcessId(), process.id());
    EXPECT_EQ(-1, process.exit_code());
    EXPECT_TRUE(-1 != process.native_handle());

    process.detach();

    EXPECT_FALSE(process.valid());
    EXPECT_FALSE(process.joinable());
    EXPECT_FALSE(process.running());
    EXPECT_TRUE (process.wait_until(0, error));

    EXPECT_EQ(-1, process.id());
    EXPECT_EQ(-1, process.native_handle());
    EXPECT_EQ(-1, process.exit_code());
}

TEST(process, create_constructor)
{
    util::platform_error error;
    {
        util::process process("C:\\Windows\\SysWOW64\\notepad.exe", L"G:\\自述文件.txt", util::process::defaulted, error);

        EXPECT_TRUE(process.valid());
        EXPECT_TRUE(process.joinable());
        EXPECT_TRUE(process.running());
        EXPECT_FALSE(process.wait_until(0, error));
        EXPECT_TRUE(!error);
    }

    {
        util::process process("", "notepad.exe", util::process::defaulted, error);

        EXPECT_TRUE(process.valid());
        EXPECT_TRUE(process.joinable());
        EXPECT_TRUE(process.running());
        EXPECT_FALSE(process.wait_until(0, error));
        EXPECT_TRUE(!error);

        EXPECT_TRUE(-1 != process.id());
        EXPECT_TRUE(-1 != process.native_handle());
        EXPECT_EQ(-1, process.exit_code());

        ::Sleep(1000);

        process.terminate(error);
        EXPECT_TRUE(process.wait_until(-1, error));
        EXPECT_TRUE(process.valid());
        EXPECT_TRUE(process.joinable());
        EXPECT_FALSE(process.running());
        EXPECT_TRUE(process.wait_until(0, error));
        EXPECT_EQ(-1, process.exit_code());

        process.join();

        EXPECT_FALSE(process.valid());
        EXPECT_EQ(-1, process.id());
        EXPECT_EQ(-1, process.native_handle());
    }
}

TEST(process, create_constructor_launch_policy)
{
    util::platform_error error;
    {
        util::process process("", "notepad.exe", util::process::with_shell, error);

        EXPECT_TRUE(process.valid());
        EXPECT_TRUE(process.joinable());
        EXPECT_TRUE(process.running());
        process.wait_until(1000, error);
        process.terminate(error);
        process.join();
    }

    {
        util::process process("", "notepad.exe", util::process::hide_window, error);

        EXPECT_TRUE(process.valid());
        EXPECT_TRUE(process.joinable());
        EXPECT_TRUE(process.running());
        process.wait_until(1000, error);
        process.terminate(error);
        process.join();
    }

    {
        util::process process("", "notepad.exe", util::process::elevate_privileges /*| util::process::hide_window*/, error);

        EXPECT_TRUE(process.valid());
        EXPECT_TRUE(process.joinable());
        EXPECT_TRUE(process.running());
        process.wait_until(1000, error);
        process.terminate(error);
        process.join();
    }
}

TEST(process, copy_constructor)
{
    util::process process;

    //util::process process_1(process);
    //util::process process_2 = process;
    //util::process process_2(std::move(process));
}