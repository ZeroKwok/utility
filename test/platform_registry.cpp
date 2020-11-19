#include <gtest/gtest.h>
#include <platform/registry_util.hpp>
#include <windows.h>

TEST(platform, registry)
{
    using namespace util::win;

    util::platform_error error;

    // 存在的子健
    EXPECT_TRUE(registry_path_exist(L"HKEY_CURRENT_USER\\Software", 0, error));
    EXPECT_TRUE(!error);

    // 不存在的子健
    EXPECT_TRUE(!registry_path_exist(L"HKEY_CURRENT_USER\\Software\\Not_Exist", 0, error));
    EXPECT_TRUE(!error);

    // 存在的值
    EXPECT_TRUE(registry_value_exist(L"HKEY_CURRENT_USER\\Environment", L"TEMP", 0, error));
    EXPECT_TRUE(!error);

    // 不存在的值
    EXPECT_TRUE(!registry_value_exist(L"HKEY_CURRENT_USER\\Environment", L"Not_Exist", 0, error));
    EXPECT_TRUE(!error);

    // 查询环境变量, 不展开
    EXPECT_TRUE(
        L"%%%%USERPROFILE%%%%\\AppData\\Local\\Temp" ==
        registry_get_expand_wstring(L"HKEY_CURRENT_USER\\Environment", L"TEMP", 0, error));

    // 查询环境变量, 展开
    EXPECT_TRUE(!registry_get_wstring(L"HKEY_CURRENT_USER\\Environment", L"TEMP", 0, error).empty());
    EXPECT_TRUE(!error);

    // 查询机器id
    // 32位下实际定位: HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Cryptography, 不存在
    EXPECT_TRUE(registry_get_wstring(L"SOFTWARE\\Microsoft\\Cryptography", L"MachineGuid", 0, error).empty());
    EXPECT_TRUE(error);

    // 查询机器id
    EXPECT_TRUE(!registry_get_wstring(L"SOFTWARE\\Microsoft\\Cryptography", L"MachineGuid", KEY_WOW64_64KEY, error).empty());
    EXPECT_TRUE(!error);

    // 查询系统名称
    // 32位下实际定位: HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Windows NT\CurrentVersion
    EXPECT_TRUE(!registry_get_wstring("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName", 0, error).empty());
    EXPECT_TRUE(!error);

    // 查询系统名称
    EXPECT_TRUE(!registry_get_wstring("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName", KEY_WOW64_64KEY, error).empty());
    EXPECT_TRUE(!error);

    // 查询系统主版本号
    EXPECT_NE(0, registry_get_dword("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "CurrentMajorVersionNumber", KEY_WOW64_64KEY, error));
    EXPECT_TRUE(!error);

    // 查询系统安装时间
    EXPECT_NE(0, registry_get_qword("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "InstallTime", KEY_WOW64_64KEY, error));
    EXPECT_TRUE(!error);

    // 查询系统DigitalProductId
    EXPECT_TRUE(!registry_get_binary("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "DigitalProductId", KEY_WOW64_64KEY, error).empty());
    EXPECT_TRUE(!error);
}


