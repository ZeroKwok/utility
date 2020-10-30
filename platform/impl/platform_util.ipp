#ifdef UTILITY_DISABLE_HEADONLY
#   include "../platform_util.hpp"
#endif

#include <windows.h>
#include <string/string_util.hpp>
#include <string/string_conv_easy.hpp>

namespace util{
namespace win {

// Returns true, if this program running on Wow64.
bool is_wow64()
{
    static bool bIsRead = false;
    static BOOL bIsWow64 = false;

    if (!bIsRead)
    {
        //32位进程运行在64位系统下返回true
        typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
        LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
            GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

        if (NULL != fnIsWow64Process)
        {
            if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
            {
                //handle error
            }
        }

        bIsRead = true;
    }

    return !!bIsWow64;
}

// Returns true, if this program running on 64bit OS.
bool is_64bitsys()
{
#if defined(_WIN64)

    return true; // 64-bit programs run only on Win64

#else // _WIN32

    // 32-bit programs run on both 32-bit and 64-bit Windows
    // so must sniff
    return is_wow64();
#endif
}

// Returns true, if this program running on the little-endian CPU.
bool is_little_endian()
{
    unsigned short buf16 = 0x00ff;
    unsigned char* buf8 = (unsigned char*)&buf16;
    return *buf8 == 0xff;
}

bool is_running_on_vista_or_higher()
{
    OSVERSIONINFOW os_version = { 0 };
    os_version.dwOSVersionInfoSize = sizeof(os_version);
    GetVersionExW(&os_version);
    return os_version.dwMajorVersion >= 6;
}

bool is_running_on_win7_or_higher()
{
    OSVERSIONINFOW os_version = { 0 };
    os_version.dwOSVersionInfoSize = sizeof(os_version);
    GetVersionExW(&os_version);

    if (os_version.dwMajorVersion > 6)
        return true;
    else if (os_version.dwMajorVersion == 6 && os_version.dwMinorVersion >= 1)
        return true;
    else
        return false;
}

bool is_running_on_win7_sp1_or_higher()
{
    OSVERSIONINFOEXW os_version = { 0 };
    os_version.dwOSVersionInfoSize = sizeof(os_version);
    GetVersionExW((OSVERSIONINFOW*)&os_version);

    if (os_version.dwMajorVersion > 6)
        return true;
    else if (os_version.dwMajorVersion == 6 && os_version.dwMinorVersion > 1)
        return true;
    else if (os_version.dwMajorVersion == 6 && os_version.dwMinorVersion == 1 && os_version.wServicePackMajor >= 1)
        return true;
    else
        return false;
}

// Determine if the user is part of the adminstators group. This will return
// true in case of XP and 2K if the user belongs to admin group. In case of
// Vista, it only returns true if the admin is running elevated.
bool is_user_admin()
{
	SID_IDENTIFIER_AUTHORITY nt_authority = SECURITY_NT_AUTHORITY;
	PSID administrators_group = NULL;
	BOOL result = ::AllocateAndInitializeSid(&nt_authority,
											 2,
											 SECURITY_BUILTIN_DOMAIN_RID,
											 DOMAIN_ALIAS_RID_ADMINS,
											 0, 0, 0, 0, 0, 0,
											 &administrators_group);
	if (result)
	{
		if (!::CheckTokenMembership(NULL, administrators_group, &result))
			result = false;
		::FreeSid(administrators_group);
	}
	return !!result;
}

// Returns true if the user is running as a non-elevated admin in case of
// Vista. In case of XP always returns false.
bool is_user_non_elevated_admin()
{
    // If pre-Vista return false;
    if (!is_running_on_vista_or_higher())
        return false;

    bool non_elevated_admin = false;
    HANDLE token = NULL;
    if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_READ, &token))
    {
        TOKEN_ELEVATION_TYPE elevation_type = TokenElevationTypeDefault;
        DWORD infoLen = 0;
        if (::GetTokenInformation(
            token,
            TokenElevationType,
            reinterpret_cast<void*>(&elevation_type),
            sizeof(elevation_type),
            &infoLen))
        {
            if (elevation_type == TokenElevationTypeLimited)
                non_elevated_admin = true;
        }
    }

    if (token)
        ::CloseHandle(token);

    return non_elevated_admin;
}

bool set_thread_name(const std::string& name, int thread_id/* = -1*/)
{
    // SEE:
    // http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
    
    // Related to Thread Name 
#pragma pack(push,8)
    typedef struct tagTHREADNAME_INFO
    {
        DWORD  dwType;       // Must be 0x1000.
        LPCSTR szName;       // Pointer to name (in user addr space).
        DWORD  dwThreadID;   // Thread ID (-1=caller thread).
        DWORD  dwFlags;      // Reserved for future use, must be zero.
    } THREADNAME_INFO;
#pragma pack(pop)

	THREADNAME_INFO info;
	info.dwType     = 0x1000;
	info.szName     = name.c_str();
	info.dwThreadID = thread_id;
	info.dwFlags    = 0;

    __try
    {
        ::RaiseException(
            0x406D1388,                     // Set a Thread Name in Native Code
            0,
            sizeof(info) / sizeof(ULONG_PTR),
            (ULONG_PTR*)&info);

        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
    }

    return false;
}

// Returns the name of the current system
// Note: https://docs.microsoft.com/zh-cn/windows/win32/sysinfo/targeting-your-application-at-windows-8-1
std::string system_name()
{
    return conv::easy::_2str(wsystem_name());
}

std::wstring wsystem_name()
{
    std::wstring name = L"Windows ?";

    OSVERSIONINFOEXW version = { 0 };
    version.dwOSVersionInfoSize = sizeof(version);
    GetVersionExW((OSVERSIONINFOW*)&version);

    if (version.dwPlatformId != VER_PLATFORM_WIN32_NT || version.dwMajorVersion < 5)
        return name;

    switch (version.dwMajorVersion)
    {
    case 5:
        switch (version.dwMinorVersion)
        {
        case 0: name = L"Windows 2000"; break;

        case 1: name = L"Windows XP"; break;

        case 2:
            if (version.wProductType == VER_NT_WORKSTATION && is_64bitsys())
                name = L"Windows XP Professional x64 Edition";

            else if (GetSystemMetrics(SM_SERVERR2) == 0)
                name = L"Windows Server 2003";

            else if (version.wSuiteMask & 0x00008000) // VER_SUITE_WH_SERVER 0x00008000
                name = L"Windows Home Server";

            else if (GetSystemMetrics(SM_SERVERR2) != 0)
                name = L"Windows Server 2003 R2";
        }
        break;

    case 6:
        switch (version.dwMinorVersion)
        {
        case 0:
            if (version.wProductType == VER_NT_WORKSTATION)
                name = L"Windows Vista";
            else
                name = L"Windows Server 2008";
            break;

        case 1:
            if (version.wProductType == VER_NT_WORKSTATION)
                name = L"Windows 7";
            else
                name = L"Windows Server 2008 R2";

        case 2:
            if (version.wProductType == VER_NT_WORKSTATION)
                name = L"Windows 8";
            else
                name = L"Windows Server 2012";
            break;

        case 3:
            if (version.wProductType == VER_NT_WORKSTATION)
                name = L"Windows 8.1";
            else
                name = L"Windows Server 2012 R2";
            break;
        }
        break;

    case 10:
        switch (version.dwMinorVersion)
        {
        case 0:
            if (version.wProductType == VER_NT_WORKSTATION)
                name = L"Windows 10";
            else
                name = L"Windows Server 2016";
        }
        break;
    }

    if (version.wServicePackMajor > 0)
        name += util::sformat(L" Service Pack %d", version.wServicePackMajor);

    if (!name.empty())
        name += is_64bitsys() ? L" x64" : L" x86";

    return name;
}

// Returns a string with the meaning of the dwError error code.
std::string format_error(int error_code)
{
    return conv::easy::_2str(wformat_error(error_code));
}

std::wstring wformat_error(int error_code)
{
    HLOCAL hlocal = NULL;

    if (!::FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL,
        error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&hlocal,
        0,
        NULL))
    {
        if (HMODULE hDll = LoadLibraryExW(
            L"netmsg.dll",
            NULL, 
            DONT_RESOLVE_DLL_REFERENCES))
        {
            ::FormatMessageW(
                FORMAT_MESSAGE_FROM_HMODULE |
                FORMAT_MESSAGE_IGNORE_INSERTS |
                FORMAT_MESSAGE_ALLOCATE_BUFFER,
                hDll,
                error_code,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPWSTR)&hlocal,
                0,
                NULL);

            ::FreeLibrary(hDll);
        }
    }

    if (hlocal != NULL)
    {
        std::wstring details = (LPWSTR)::LocalLock(hlocal);
        ::LocalUnlock(hlocal);
        if (!details.empty())
        {
            // \r
            if (details[details.length() - 1] == 0x0a)
                details.erase(details.end() - 1);

            // \n
            if (details[details.length() - 1] == 0x0d)
                details.erase(details.end() - 1);
        }

        return details;
    }

    return std::wstring(L"Error message not available.");
}

// Determine the mandatory level of a SID
HRESULT get_sid_integrity_level(PSID sid, MANDATORY_LEVEL* level)
{
    static SID_IDENTIFIER_AUTHORITY mandatory_label_auth =
        SECURITY_MANDATORY_LABEL_AUTHORITY;

    if (!IsValidSid(sid))
        return E_FAIL;

    SID_IDENTIFIER_AUTHORITY* authority = ::GetSidIdentifierAuthority(sid);
    if (!authority)
        return E_FAIL;

    if (memcmp(authority, &mandatory_label_auth, sizeof(SID_IDENTIFIER_AUTHORITY)))
        return E_FAIL;

    PUCHAR count = ::GetSidSubAuthorityCount(sid);
    if (!count || *count != 1)
        return E_FAIL;

    DWORD* rid = ::GetSidSubAuthority(sid, 0);
    if (!rid)
        return E_FAIL;

    if ((*rid & 0xFFF) != 0 || *rid > SECURITY_MANDATORY_PROTECTED_PROCESS_RID)
        return E_FAIL;

    *level = static_cast<MANDATORY_LEVEL>(*rid >> 12);
    return S_OK;
}

// Determine the mandatory level of a process
// processID, the process to query, or (0) to use the current process
// On Vista, level should always be filled in with either
//     MandatoryLevelLow (IE)
//     MandatoryLevelMedium(user), or
//     MandatoryLevelHigh( Elevated Admin)
// On error, level remains unchanged
HRESULT get_process_integrity_level(DWORD process_id, MANDATORY_LEVEL* level)
{
    if (!is_running_on_vista_or_higher())
        return E_NOTIMPL;

    if (process_id == 0)
        process_id = ::GetCurrentProcessId();

    HRESULT result = E_FAIL;
    HANDLE process = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, process_id);

    if (process != NULL)
    {
        HANDLE current_token;
        if (::OpenProcessToken(process,
            TOKEN_QUERY | TOKEN_QUERY_SOURCE,
            &current_token))
        {
            DWORD label_size = 0;
            TOKEN_MANDATORY_LABEL* label;
            ::GetTokenInformation(current_token,
                TokenIntegrityLevel,
                NULL,
                0,
                &label_size);
            if (label_size && (label = reinterpret_cast<TOKEN_MANDATORY_LABEL*>
                (::LocalAlloc(LPTR, label_size))) != NULL)
            {
                if (::GetTokenInformation(current_token,
                    TokenIntegrityLevel,
                    label,
                    label_size,
                    &label_size))
                {
                    result = get_sid_integrity_level(label->Label.Sid, level);
                }
                ::LocalFree(label);
            }
            ::CloseHandle(current_token);
        }
        ::CloseHandle(process);
    }

    return result;
}

} // win
} // util
