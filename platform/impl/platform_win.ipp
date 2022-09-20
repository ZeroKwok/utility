#ifdef UTILITY_DISABLE_HEADONLY
#   include "../platform_win.h"
#endif

#include <windows.h>
#include <string/string_util.h>
#include <string/string_conv_easy.hpp>
#include <platform/registry_win.h>

#include "dbghelp_api.hpp"
#include <boost/lexical_cast.hpp>

#if _WIN32_WINNT < _WIN32_WINNT_VISTA
#   include <winsock2.h>
#   include <ws2tcpip.h>
#endif

namespace util{
namespace win {

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

// 
// GetVersionExW()只支持到Win8.1, 此后该方法获得的版本号最大仅为Win 8.1.
// 
// Applications not manifested for Windows 8.1 or Windows 10 will return the Windows 8 OS version value (6.2). 
// Once an application is manifested for a given operating system version, 
// GetVersionEx will always return the version that the application is manifested for in future releases. 
// https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getversionexw
//
OSVERSIONINFOEXW MyGetVersionExW()
{
    OSVERSIONINFOEXW version = { 0 };
    version.dwOSVersionInfoSize = sizeof(version);
    GetVersionExW((OSVERSIONINFOW*)&version);

    if (auto hinst = LoadLibraryA("ntdll.dll"))
    {
        typedef void (WINAPI* getver)(DWORD*, DWORD*, DWORD*);
        if (getver proc = (getver)GetProcAddress(hinst, "RtlGetNtVersionNumbers"))
        {
            proc(&version.dwMajorVersion, &version.dwMinorVersion, &version.dwBuildNumber);
            version.dwBuildNumber = version.dwBuildNumber & 0xffff;
        }

        FreeLibrary(hinst);
    }

    return version;
}

// Returns true, if this program running on Wow64.
// That is, 32-bit programs run on 64-bit systems.
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

bool is_running_on_win11_or_higher()
{
    OSVERSIONINFOEXW version = MyGetVersionExW();
    if (version.dwMajorVersion > 10)
        return true;
    if (version.dwMajorVersion == 10 && version.dwBuildNumber >= 22000)
        return true;

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

#if _WIN32_WINNT < _WIN32_WINNT_VISTA
//
// https://stackoverflow.com/questions/13731243/what-is-the-windows-xp-equivalent-of-inet-pton-or-inetpton
// 
inline int inet_pton(int af, const char* src, void* dst)
{
    struct sockaddr_storage ss;
    int size = sizeof(ss);
    char src_copy[INET6_ADDRSTRLEN + 1];

    ZeroMemory(&ss, sizeof(ss));
    /* stupid non-const API */
    strncpy(src_copy, src, INET6_ADDRSTRLEN + 1);
    src_copy[INET6_ADDRSTRLEN] = 0;

    if (WSAStringToAddress(src_copy, af, NULL, (struct sockaddr*)&ss, &size) == 0) {
        switch (af) {
        case AF_INET:
            *(struct in_addr*)dst = ((struct sockaddr_in*)&ss)->sin_addr;
            return 1;
        case AF_INET6:
            *(struct in6_addr*)dst = ((struct sockaddr_in6*)&ss)->sin6_addr;
            return 1;
        }
    }
    return 0;
}
#endif

bool is_network_available()
{
    static struct _internal
    {
        _internal()
        {
            WSADATA wsaData = { 0 };
            WSAStartup(MAKEWORD(2, 2), &wsaData);
        }
    } _init;

    sockaddr_in hostaddr = { 0 };
    hostaddr.sin_family  = AF_INET;
    hostaddr.sin_port    = htons(53);

    const char *addresses[] = {
        "1.1.1.1",
        "223.5.5.5",
        nullptr
    };

    for (int i = 0; addresses[i] != nullptr; ++i)
    {
        inet_pton(AF_INET, addresses[i], &hostaddr.sin_addr);

        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0)
            return false;

        // 设置阻塞模式
        unsigned long mode = 1;
        ioctlsocket(fd, FIONBIO, &mode);

        // 尝试连接
        connect(fd, (sockaddr*)&hostaddr, sizeof(hostaddr));

        timeval timeout = {
            /* tv_sec  */ 0,
            /* tv_usec */ 300000
        };

        fd_set fd_write, fd_error;

        FD_ZERO(&fd_write);
        FD_ZERO(&fd_error);

        FD_SET(fd, &fd_write);
        FD_SET(fd, &fd_error);

        select(0, 0, &fd_write, &fd_error, &timeout);
        closesocket(fd);

        if (FD_ISSET(fd, &fd_write))
            return true;
    }

    return false;
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
    platform_error error, error1;

    // 优先读取注册表
    std::wstring name = registry_get_wstring(
        L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 
        L"ProductName", 
        is_wow64() ? KEY_WOW64_64KEY : 0, 
        error);

    std::wstring build = registry_get_wstring(
        L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
        L"CurrentBuildNumber",
        is_wow64() ? KEY_WOW64_64KEY : 0,
        error1);

    int number = 0;
    try {
        number = boost::lexical_cast<int>(build);
    }
    catch (...) {
    }

    if (!error)
    {
        // 判断是否为 Win 11
        if (number >= 22000 && name.find(L"Windows 10") != name.npos)
        {
            util::replace(name, L"Windows 10", L"Windows 11");
        }
    }
    else
    {
        OSVERSIONINFOEXW version = MyGetVersionExW();

        name = L"Windows ?";
        if (version.dwPlatformId == VER_PLATFORM_WIN32_NT && version.dwMajorVersion >= 5)
        {
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
                    {
                        // 判断Win 11
                        if (version.dwBuildNumber >= 22000)
                            name = L"Windows 11";
                        else
                            name = L"Windows 10";
                    }
                    else
                        name = L"Windows Server 2016";
                }
                break;
            }

            if (version.wServicePackMajor > 0)
                name += util::sformat(L" Service Pack %d", version.wServicePackMajor);
        }
    }

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

bool is_network_error(int error_code)
{
    switch (error_code)
    {
    case ERROR_REM_NOT_LIST:
    case ERROR_DUP_NAME:
    case ERROR_BAD_NETPATH:
    case ERROR_NETWORK_BUSY:
    case ERROR_DEV_NOT_EXIST:
    case ERROR_TOO_MANY_CMDS:
    case ERROR_ADAP_HDW_ERR:
    case ERROR_BAD_NET_RESP:
    case ERROR_UNEXP_NET_ERR:
    case ERROR_NETNAME_DELETED:
    case ERROR_NETWORK_ACCESS_DENIED:
    case ERROR_BAD_DEV_TYPE:
    case ERROR_BAD_NET_NAME:
    case ERROR_TOO_MANY_NAMES:
    case ERROR_TOO_MANY_SESS:
    case ERROR_SHARING_PAUSED:
    
    // 2021-6-21 添加
    case ERROR_NO_NETWORK:
    case ERROR_CONNECTION_REFUSED:
    case ERROR_CONNECTION_INVALID:
    case ERROR_NETWORK_UNREACHABLE:
    case ERROR_HOST_UNREACHABLE:
    case ERROR_PROTOCOL_UNREACHABLE:
    case ERROR_PORT_UNREACHABLE:
    case ERROR_REQUEST_ABORTED:
    case ERROR_CONNECTION_ABORTED:
    case ERROR_CONNECTION_COUNT_LIMIT:
        return true;
    }

    return false;
}

bool is_supported_stack_trace()
{
#if (NTDDI_VERSION > NTDDI_WINXP)
    return winapi::init_dbghelp_api();
#else
    return false;
#endif
}

bool get_stack_frame(call_stack_t& frames, int skip/* = 0*/, int capture/* = 5*/)
{
    if (!is_supported_stack_trace())
        return false;

#if (NTDDI_VERSION > NTDDI_WINXP)
    HANDLE hProcess = ::GetCurrentProcess();

    //
    // https://docs.microsoft.com/zh-cn/windows/win32/api/dbghelp/nf-dbghelp-syminitializew

    winapi::fnSymInitializeW(hProcess, NULL, TRUE);

    intptr_t* traces = new intptr_t[capture];
    WORD frameCount = ::CaptureStackBackTrace(skip + 1, capture, (PVOID*)traces, NULL);

    for (WORD i = 0; i < frameCount; ++i)
    {
        //stack frame
        stack_frame_t sf = {};

        //module
        {
            IMAGEHLP_MODULEW64 moduleInfo = {};
            moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULEW64);

            //
            // https://docs.microsoft.com/zh-cn/windows/win32/api/dbghelp/nf-dbghelp-symgetmoduleinfow64

            if (winapi::fnSymGetModuleInfoW64(hProcess, (DWORD64)traces[i], &moduleInfo))
            {
                sf.module_name = moduleInfo.ImageName;
            }
        }

        //symbol
        {
            DWORD64 displacementSym = 0;
            char buffer[sizeof(SYMBOL_INFOW) + MAX_SYM_NAME * sizeof(wchar_t)] = { 0 };

            PSYMBOL_INFOW symbol = (PSYMBOL_INFOW)buffer;
            symbol->SizeOfStruct = sizeof(SYMBOL_INFOW);
            symbol->MaxNameLen = MAX_SYM_NAME;

            //
            // https://docs.microsoft.com/zh-cn/windows/win32/api/dbghelp/nf-dbghelp-symfromaddrw

            if (winapi::fnSymFromAddrW(hProcess, (DWORD64)traces[i], &displacementSym, symbol))
            {
                sf.symbol_name = symbol->Name;
                sf.symbol_address = symbol->Address;
            }
        }

        //line
        {
            DWORD displacementLine = 0;

            IMAGEHLP_LINEW64 line = {};
            line.SizeOfStruct = sizeof(IMAGEHLP_LINEW64);

            // 
            // https://docs.microsoft.com/zh-cn/windows/win32/api/dbghelp/nf-dbghelp-symgetlinefromaddrw64

            if (winapi::fnSymGetLineFromAddrW64(hProcess, (DWORD64)traces[i], &displacementLine, &line))
            {
                sf.file_name = line.FileName;
                sf.line_number = line.LineNumber;
            }
        }

        frames.push_back(sf);
    }

    ::CloseHandle(hProcess);

    delete[] traces;

    return frameCount > 0;
#endif
}

// Returns the filename part of the path
// From file_util.hpp
template<class _TChar>
inline std::basic_string<_TChar> get_filename(const std::basic_string<_TChar>& path)
{
    size_t pos = path.npos;
    size_t pos1 = path.rfind('\\');
    size_t pos2 = path.rfind('/');

    if (pos1 == path.npos)
        pos = pos2;
    else if (pos2 == path.npos)
        pos = pos1;
    else
        pos = std::max(pos1, pos2);

    return path.substr(pos + 1);
}

std::wstring& format_stack_trace(const call_stack_t& frames, std::wstring& trace/* = std::wstring()*/)
{
    // Define a maximum number of blank
    static const int max_space = 45;

    // Print header information
    trace += util::sformat(L"Thread[%x] stack tracebacks:\n", ::GetCurrentThreadId());

    for (call_stack_t::const_iterator f = frames.begin(); f != frames.end(); ++f)
    {
        std::wstring symbol = f->symbol_name.empty() ? util::sformat(L"%x", f->symbol_address)
            : f->symbol_name;

        symbol = get_filename(f->module_name) + L"!" + symbol;
        trace += symbol;

        if (!f->file_name.empty())
        {
            trace += std::wstring(std::max<int>(max_space - int(symbol.size()), 1), L' ');
            trace += util::sformat(L"at %s:%d", get_filename(f->file_name).data(), f->line_number);
        }

        trace += '\n';
    }

    return trace;
}

std::wstring stack_trace(int skip/* = 0*/, int capture/* = 5*/)
{
    call_stack_t frames;
    if (!get_stack_frame(frames, skip + 1, capture))
        return L"Cannot read stack tracebacks";

    std::wstring result;
    return format_stack_trace(frames, result);
}

} // win
} // util