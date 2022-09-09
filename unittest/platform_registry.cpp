#include <gtest/gtest.h>
#include <platform/registry_win.h>
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
        L"%USERPROFILE%\\AppData\\Local\\Temp" ==
        registry_get_expand_wstring(L"HKEY_CURRENT_USER\\Environment", L"TEMP", 0, error));

    // 查询环境变量, 展开
    EXPECT_TRUE(!registry_get_wstring(L"HKEY_CURRENT_USER\\Environment", L"TEMP", 0, error).empty());
    EXPECT_TRUE(!error);

#if ARCH_CPU_32_BITS
    // 查询机器id
    // 32位下实际定位: HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Cryptography, 不存在
    EXPECT_TRUE(registry_get_wstring(L"SOFTWARE\\Microsoft\\Cryptography", L"MachineGuid", 0, error).empty());
    EXPECT_TRUE(error);
#endif

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

    // 如果存在则删除
    if (registry_path_exist("HKEY_CURRENT_USER\\SOFTWARE\\utility", 0, error))
    {
        // 删除path
        registry_directory_remove("HKEY_CURRENT_USER\\SOFTWARE\\utility", 0, error);
        EXPECT_TRUE(!error);
    }

    // 创建subkey并且设置dword
    registry_set_dword("HKEY_CURRENT_USER\\SOFTWARE\\utility", "dword", 0xff32, 0, error);
    EXPECT_TRUE(!error);

    // 设置dword
    registry_set_qword("HKEY_CURRENT_USER\\SOFTWARE\\utility", "qword", 0xff32aa55, 0, error);
    EXPECT_TRUE(!error);

    // 设置二进制
    registry_set_binary("HKEY_CURRENT_USER\\SOFTWARE\\utility", "binary", util::bytedata("hello binary"), 0, error);
    EXPECT_TRUE(!error);

    // 设置wstring
    registry_set_wstring("HKEY_CURRENT_USER\\SOFTWARE\\utility", "wstring", std::wstring(L"hello wstring"), 0, error);
    EXPECT_TRUE(!error);

    // 设置mutil_string
    std::vector<std::wstring> mutil_string;
    mutil_string.push_back(L"hello mutil_string");
    mutil_string.push_back(L"hello mutil_string 01");
    mutil_string.push_back(L"hello mutil_string 02");
    registry_set_multi_wstring("HKEY_CURRENT_USER\\SOFTWARE\\utility", "mutil_string", mutil_string, 0, error);
    EXPECT_TRUE(!error);

    // 查询该路径下所有顶级key
    auto keys = registry_get_wkeys("HKEY_CURRENT_USER\\SOFTWARE", 0);
    EXPECT_FALSE(keys.empty());

    auto vaules = registry_get_wvalues("HKEY_CURRENT_USER\\SOFTWARE\\utility", 0);
    EXPECT_EQ(vaules.size(), 5);

    // 删除所有已创建的值
    const char* value_names[] = {
        "dword",
        "qword",
        "binary",
        "wstring",
        "mutil_string",
        nullptr
    };

    for (int i = 0; value_names[i] != nullptr; ++i)
    {
        if (registry_value_exist("HKEY_CURRENT_USER\\SOFTWARE\\utility", value_names[i], 0, error))
        {
            registry_value_remove("HKEY_CURRENT_USER\\SOFTWARE\\utility", value_names[i], 0, error);
            EXPECT_TRUE(!error);
        }
        else
        {
            EXPECT_TRUE(false);
        }
    }

    // 删除已创建的key
    if (registry_path_exist("HKEY_CURRENT_USER\\SOFTWARE\\utility", 0, error))
    {
        // 删除path
        registry_directory_remove("HKEY_CURRENT_USER\\SOFTWARE\\utility", 0, error);
        EXPECT_TRUE(!error);
    }
    else
    {
        EXPECT_TRUE(false);
    }

    // 创建多级subkey
    registry_set_wstring("HKEY_CURRENT_USER\\SOFTWARE\\utility\\test\\3rd\\long_long", "wstring", L"hello wstring", 0, error);
    EXPECT_TRUE(!error);

    // 删除已创建的key
    if (registry_path_exist("HKEY_CURRENT_USER\\SOFTWARE\\utility", 0, error))
    {
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
        // 删除path
        registry_tree_remove("HKEY_CURRENT_USER\\SOFTWARE\\utility", error);
        EXPECT_TRUE(!error);
#endif
    }
    else
    {
        EXPECT_TRUE(false);
    }

}
