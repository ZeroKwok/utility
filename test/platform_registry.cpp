#include <gtest/gtest.h>
#include <platform/registry_util.hpp>
#include <windows.h>

TEST(platform, registry)
{
    using namespace util::win;

    util::platform_error error;

    // ���ڵ��ӽ�
    EXPECT_TRUE(registry_path_exist(L"HKEY_CURRENT_USER\\Software", 0, error));
    EXPECT_TRUE(!error);

    // �����ڵ��ӽ�
    EXPECT_TRUE(!registry_path_exist(L"HKEY_CURRENT_USER\\Software\\Not_Exist", 0, error));
    EXPECT_TRUE(!error);

    // ���ڵ�ֵ
    EXPECT_TRUE(registry_value_exist(L"HKEY_CURRENT_USER\\Environment", L"TEMP", 0, error));
    EXPECT_TRUE(!error);

    // �����ڵ�ֵ
    EXPECT_TRUE(!registry_value_exist(L"HKEY_CURRENT_USER\\Environment", L"Not_Exist", 0, error));
    EXPECT_TRUE(!error);

    // ��ѯ��������, ��չ��
    EXPECT_TRUE(
        L"%USERPROFILE%\\AppData\\Local\\Temp" ==
        registry_get_expand_wstring(L"HKEY_CURRENT_USER\\Environment", L"TEMP", 0, error));

    // ��ѯ��������, չ��
    EXPECT_TRUE(!registry_get_wstring(L"HKEY_CURRENT_USER\\Environment", L"TEMP", 0, error).empty());
    EXPECT_TRUE(!error);

    // ��ѯ����id
    // 32λ��ʵ�ʶ�λ: HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Cryptography, ������
    EXPECT_TRUE(registry_get_wstring(L"SOFTWARE\\Microsoft\\Cryptography", L"MachineGuid", 0, error).empty());
    EXPECT_TRUE(error);

    // ��ѯ����id
    EXPECT_TRUE(!registry_get_wstring(L"SOFTWARE\\Microsoft\\Cryptography", L"MachineGuid", KEY_WOW64_64KEY, error).empty());
    EXPECT_TRUE(!error);

    // ��ѯϵͳ����
    // 32λ��ʵ�ʶ�λ: HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Windows NT\CurrentVersion
    EXPECT_TRUE(!registry_get_wstring("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName", 0, error).empty());
    EXPECT_TRUE(!error);

    // ��ѯϵͳ����
    EXPECT_TRUE(!registry_get_wstring("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName", KEY_WOW64_64KEY, error).empty());
    EXPECT_TRUE(!error);

    // ��ѯϵͳ���汾��
    EXPECT_NE(0, registry_get_dword("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "CurrentMajorVersionNumber", KEY_WOW64_64KEY, error));
    EXPECT_TRUE(!error);

    // ��ѯϵͳ��װʱ��
    EXPECT_NE(0, registry_get_qword("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "InstallTime", KEY_WOW64_64KEY, error));
    EXPECT_TRUE(!error);

    // ��ѯϵͳDigitalProductId
    EXPECT_TRUE(!registry_get_binary("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "DigitalProductId", KEY_WOW64_64KEY, error).empty());
    EXPECT_TRUE(!error);

    // ���������ɾ��
    if (registry_path_exist("HKEY_CURRENT_USER\\SOFTWARE\\utility", 0, error))
    {
        // ɾ��path
        registry_directory_remove("HKEY_CURRENT_USER\\SOFTWARE\\utility", 0, error);
        EXPECT_TRUE(!error);
    }

    // ����subkey��������dword
    registry_set_dword("HKEY_CURRENT_USER\\SOFTWARE\\utility", "dword", 0xff32, 0, error);
    EXPECT_TRUE(!error);

    //����dword
    registry_set_qword("HKEY_CURRENT_USER\\SOFTWARE\\utility", "qword", 0xff32aa55, 0, error);
    EXPECT_TRUE(!error);

    //���ö�����
    registry_set_binary("HKEY_CURRENT_USER\\SOFTWARE\\utility", "binary", util::bytedata("hello binary"), 0, error);
    EXPECT_TRUE(!error);

    //����wstring
    registry_set_wstring("HKEY_CURRENT_USER\\SOFTWARE\\utility", "wstring", std::wstring(L"hello wstring"), 0, error);
    EXPECT_TRUE(!error);

    //����mutil_string
    std::vector<std::wstring> mutil_string;
    mutil_string.push_back(L"hello mutil_string");
    mutil_string.push_back(L"hello mutil_string 01");
    mutil_string.push_back(L"hello mutil_string 02");
    registry_set_multi_wstring("HKEY_CURRENT_USER\\SOFTWARE\\utility", "mutil_string", mutil_string, 0, error);
    EXPECT_TRUE(!error);

    // ɾ�������Ѵ�����ֵ
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

    // ɾ���Ѵ�����key
    if (registry_path_exist("HKEY_CURRENT_USER\\SOFTWARE\\utility", 0, error))
    {
        // ɾ��path
        registry_directory_remove("HKEY_CURRENT_USER\\SOFTWARE\\utility", 0, error);
        EXPECT_TRUE(!error);
    }
    else
    {
        EXPECT_TRUE(false);
    }

    // �����༶subkey
    registry_set_wstring("HKEY_CURRENT_USER\\SOFTWARE\\utility\\test\\3rd\\long_long", "wstring", L"hello wstring", 0, error);
    EXPECT_TRUE(!error);

    // ɾ���Ѵ�����key
    if (registry_path_exist("HKEY_CURRENT_USER\\SOFTWARE\\utility", 0, error))
    {
        // ɾ��path
        registry_tree_remove("HKEY_CURRENT_USER\\SOFTWARE\\utility", error);
        EXPECT_TRUE(!error);
    }
    else
    {
        EXPECT_TRUE(false);
    }
}


