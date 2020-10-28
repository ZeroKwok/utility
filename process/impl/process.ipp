#ifdef UTILITY_DISABLE_HEADONLY
#   include "../process.hpp"
#endif

#include <windows.h>
#include <shellapi.h>
#include <string/string_util.hpp>
#include <platform/platform_util.hpp>

namespace util {
namespace detail {
  
// ֱ������
// app  �������ϵͳ����Ŀ¼, ��ʹ�õ�ǰ�������͵�ǰĿ¼, ���������չ��, ����Ϊ��, 
// cmd  �����ϵͳ����Ŀ¼, ��ٶ�.exe, ����Ϊ��
inline HANDLE runapp_with_redirection(
    const util::tstring& app,
    const util::tstring& cmd,
    HANDLE input,
    HANDLE output,
    HANDLE error,
    process::launch_policys policy,
    platform_error& perror)
{
    STARTUPINFOW si = {0};
    PROCESS_INFORMATION pi = {0};

    if (!!input || !!output || !!error)
        si.dwFlags = STARTF_USESTDHANDLES;

    si.cb			= sizeof(si);
    si.hStdInput	= input  ? input  : ::GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput	= output ? output : ::GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError	= error  ? error  : ::GetStdHandle(STD_ERROR_HANDLE);

    if (policy & process::hide_window)
    {
        si.dwFlags     = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;

        // See:
        // https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-startupinfow
    }

    std::wstring cmd_dup(cmd);

    // CREATE_NO_WINDOW
    // ��������ǿ���̨����, ���Բ���ʾ����̨�ķ�ʽ����. ���Ӧ�ó����ǿ���̨�������
    // ʹ��CREATE_NEW_CONSOLE, DETACHED_PROCESS�ñ�־��������;
    // See:
    // https://docs.microsoft.com/en-us/windows/win32/procthread/process-creation-flags

    if (::CreateProcessW(
        /* lpApplicationName,   */ app.empty()     ? nullptr : app.c_str(),
        /* lpCommandLine,       */ cmd_dup.empty() ? nullptr : &cmd_dup[0],
        /* lpProcessAttributes, */ nullptr,
        /* lpThreadAttributes,  */ nullptr,
        /* bInheritHandles,     */ (si.dwFlags & STARTF_USESTDHANDLES) ? TRUE : FALSE,
        /* dwCreationFlags,     */ (policy & process::hide_window ? CREATE_NO_WINDOW : 0), 
        /* lpEnvironment,       */ nullptr,
        /* lpCurrentDirectory,  */ nullptr,
        /* lpStartupInfo,       */ &si,
        /* lpProcessInformation */ &pi) == 0)
    {
        perror = platform_error(::GetLastError(),
            util::sformat("Can't create process with cmd[%s %s]", 
                app.string().c_str(),
                cmd.string().c_str()));

        return nullptr;
    }

    ::CloseHandle(pi.hThread);

    return pi.hProcess;
}

// ͨ��shell����, ��Ҫ������Ȩ
// app û�а���·��, ��ʹ�õ�ǰĿ¼, ����Ϊ��;
// cmd �����в���, ����Ϊ��;
inline HANDLE runapp_shell_and_elevate_privileges(
    util::tstring app,
    util::tstring cmd,
    process::launch_policys policy,
    platform_error& error)
{
    /*
    *   ShellExecuteExW��CreateProcessW��ͬ����lpFile�ֶα�����ֵ
    *   �����ڲ���lpParameters��ָ����ִ��ģ��
    *   ����Ϊ�˼���RunAppWithRedirection, ��Ҫ�ֶ��ָ����cmd�е�
    *   ģ����
    *   2018-4-12 By GuoJH
    */
    
    // ɾ����ͷ�ո�
    while (!cmd.empty() && cmd[0] == L' ')
        cmd.erase(cmd.begin());

    if (app.empty() && !cmd.empty())
    {
        // ��ʼ����˫����, ˵����ִ��ģ�������ŷָ�, �������Կո�ָ�
        util::tstring::size_type index = 0;

        if (cmd[0] == L'\"')
        {
            if ((index = cmd.find(L'\"', 1)) != util::tstring::npos)
            {
                app = cmd.substr(0, index + 1);
                cmd = cmd.substr(app.length());
            }
        }
        else
        {
            if ((index = cmd.find(L' ')) != util::tstring::npos)
            {
                app = cmd.substr(0, index + 1);
                cmd = cmd.substr(app.length());
            }
            else
            {
                app.swap(cmd);  // cmd �в������ո�, ˵�����п�ִ���ļ�
            }
        }

        if (app.empty())
        {
            error = platform_error(-1,
                util::sformat("Invalid parameter, can't create process with cmd[%s %s]",
                    app.string().c_str(),
                    cmd.string().c_str()));
            return nullptr;
        }
    }

    // see:
    // https://docs.microsoft.com/en-us/windows/win32/api/shellapi/ns-shellapi-shellexecuteinfoa

    SHELLEXECUTEINFOW exec_info = {0};
    exec_info.cbSize        = sizeof SHELLEXECUTEINFOW;
    exec_info.lpFile        = app.c_str();
    exec_info.lpParameters  = cmd.empty() ? NULL : cmd.c_str();
    exec_info.fMask         = SEE_MASK_NOCLOSEPROCESS;  // ���رս��̾��
    exec_info.lpVerb        = policy & process::elevate_privileges ? L"runas" : L"open";
    exec_info.nShow         = policy & process::hide_window ? SW_HIDE : SW_SHOWDEFAULT;

    if (!::ShellExecuteExW(&exec_info)) 
    {
        error = platform_error(::GetLastError(),
            util::sformat("Can't create process with cmd[%s %s]",
                app.string().c_str(),
                cmd.string().c_str()));
        return nullptr;
    }

    return exec_info.hProcess;
}

// �Է���Ȩ�ķ�ʽ����, ��Ҫ���ڽ���Ȩ��
// app  �������ϵͳ����Ŀ¼, ��ʹ�õ�ǰ�������͵�ǰĿ¼, ���������չ��, ����Ϊ��, 
// cmd  �����ϵͳ����Ŀ¼, ��ٶ�.exe, ����Ϊ��
inline HANDLE runapp_with_non_elevate_privileges(
    const util::tstring& app,
    const util::tstring& cmd,
    HANDLE input,
    HANDLE output,
    HANDLE error,
    process::launch_policys policy,
    platform_error& perror)
{ 
    // vista ������, ���� �������Ĺ���Ա
    if(!win::is_running_on_vista_or_higher() || win::is_user_non_elevated_admin())
    {
        return runapp_with_redirection(app, cmd, input, output, error, policy, perror);
    }

    // ͨ����̬���صķ�ʽʹ�øú���, Ŀ�����ڱ���vista����û�иú����Ļ�����
    // ����ȷ����������;
    typedef BOOL (WINAPI *func_CreateProcessWithTokenW)(
        __in        HANDLE hToken,
        __in        DWORD dwLogonFlags,
        __in_opt    LPCWSTR lpApplicationName,
        __inout_opt LPWSTR lpCommandLine,
        __in        DWORD dwCreationFlags,
        __in_opt    LPVOID lpEnvironment,
        __in_opt    LPCWSTR lpCurrentDirectory,
        __in        LPSTARTUPINFOW lpStartupInfo,
        __out       LPPROCESS_INFORMATION lpProcessInformation
        );

    HMODULE hMoudle = ::LoadLibraryA("Advapi32.dll");
    if (hMoudle == nullptr)
    {
        perror = platform_error(::GetLastError(), "Can't load the Advapi32.dll");
        return nullptr;
    }

    func_CreateProcessWithTokenW pCreateProcessWithTokenW = 
        (func_CreateProcessWithTokenW)::GetProcAddress(hMoudle, "CreateProcessWithTokenW");
    if(pCreateProcessWithTokenW == nullptr)
    {
        perror = platform_error(::GetLastError(), "Can't load the function CreateProcessWithTokenW");

        ::FreeLibrary(hMoudle);

        return nullptr;
    }

    STARTUPINFOW si = {0};
    PROCESS_INFORMATION pi = {0};

    if (!!input || !!output || !!error)
        si.dwFlags = STARTF_USESTDHANDLES;

    si.cb			= sizeof(si);
    si.hStdInput	= input  ? input  : ::GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput	= output ? output : ::GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError	= error  ? error  : ::GetStdHandle(STD_ERROR_HANDLE);

    if (policy & process::hide_window)
    {
        si.dwFlags      = STARTF_USESHOWWINDOW;
        si.wShowWindow  = SW_HIDE;
    }

    std::wstring cmd_dup(cmd);

    // ���explorer����Pid
    DWORD explorerPid = 0;

    ::GetWindowThreadProcessId(::GetShellWindow(), &explorerPid);

    // ATTENTION:
    // If UAC is turned OFF all processes run with SECURITY_MANDATORY_HIGH_RID, also Explorer!
    // But this does not matter because to start the new process without UAC no elevation is required.
    HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, explorerPid);

    if(!hProcess)
        goto _CleanUp;

    // �򿪽���token, ��������
    HANDLE hToken = 0;
    HANDLE hToken2 = 0;
    if(!::OpenProcessToken(hProcess, TOKEN_DUPLICATE, &hToken))
        goto _CleanUp;

    if (!::DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, 0, SecurityImpersonation, TokenPrimary, &hToken2))
        goto _CleanUp;

    // ͨ�����Ƶ�������������
    if(pCreateProcessWithTokenW(
        /* hToken,              */ hToken2,
        /* dwLogonFlags,        */ 0, 
        /* lpApplicationName,   */ app.empty()     ? nullptr : app.c_str(),
        /* lpCommandLine,       */ cmd_dup.empty() ? nullptr : &cmd_dup[0],
        /* dwCreationFlags,     */ (policy & process::hide_window ? CREATE_NO_WINDOW : 0),
        /* lpEnvironment,       */ nullptr,
        /* lpCurrentDirectory,  */ nullptr,
        /* lpStartupInfo,       */ &si,
        /* lpProcessInformation */ &pi))
    {
        ::CloseHandle(pi.hThread);
    }
 
_CleanUp:

    // ������
    if (pi.hProcess == nullptr)
    {
        perror = platform_error(::GetLastError(),
            util::sformat("Can't create process with cmd[%s %s]",
                app.string().c_str(),
                cmd.string().c_str()));
    }

    if(hToken)
        ::CloseHandle(hToken);
    if(hToken2)
        ::CloseHandle(hToken2);
    if(hProcess)
        ::CloseHandle(hProcess);
    if(hMoudle)
        ::FreeLibrary(hMoudle);

    return pi.hProcess;
}

} // detail

process::process()
    : _handle(0)
{
}

process::process(pid_t pid, platform_error& error)
{
    error.clear();

    DWORD flags = PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE | SYNCHRONIZE;

    if (!(_handle = reinterpret_cast<native_handle_t>(::OpenProcess(flags, FALSE, pid))))
    {
        error = platform_error(::GetLastError(), "Can't open the process with the pid");
    }
}

process::process(
    const util::tstring& app,
    const util::tstring& cmd,
    launch_policys policy,
    platform_error& error)
{
    error.clear();

    if (policy & elevate_privileges && win::is_user_admin())
        policy = (policy & (~elevate_privileges));

    if (policy & depress_privileges && win::is_user_non_elevated_admin())
        policy = (policy & (~depress_privileges));

    if (policy & with_shell || policy & elevate_privileges)
    {
        _handle = reinterpret_cast<native_handle_t>(
            detail::runapp_shell_and_elevate_privileges(app, cmd, policy, error));
    }
    else if (policy & depress_privileges)
    {
        _handle = reinterpret_cast<native_handle_t>(
            detail::runapp_with_non_elevate_privileges(
            app, cmd, nullptr, nullptr, nullptr, policy, error));
    }
    else
    {
        _handle = reinterpret_cast<native_handle_t>(
            detail::runapp_with_redirection(
            app, cmd, nullptr, nullptr, nullptr, policy, error));
    }
}

process::~process()
{
    if (valid() && joinable())
    {
#ifdef OS_WIN
        ::OutputDebugStringA("*** Warnings ***\r\n");
        ::OutputDebugStringA("The process instance will be destroyed, but the process is still vaild.\r\n");
#endif
        std::terminate();
    }
}

bool process::valid() const
{
    return _handle != 0;
}

process::pid_t process::id() const
{
    if (!valid())
        return -1;
    return ::GetProcessId((HANDLE)_handle);
}

util::process::native_handle_t process::native_handle() const
{
    if (valid())
        return _handle;

    return -1;
}

int process::exit_code() const
{
    if (valid())
    {
        DWORD code = 0;
        if (GetExitCodeProcess((HANDLE)_handle, &code))
        {
            if (code != STILL_ACTIVE) // ����û����ֹ, ���� STILL_ACTIVE
                return code;
        }
    }

    return -1;
}

void process::detach()
{
    if (valid())
    {
        ::CloseHandle((HANDLE)_handle);
        _handle = 0;
    }
}

void process::join()
{
    if (joinable())
    {
        if (id() != ::GetCurrentProcessId())
        {
            platform_error error;
            wait_until(-1, error);
        }
    }

    detach();
}

bool process::joinable()
{
    return valid();
}

bool process::running()
{
    if (joinable())
    {
        if (id() == ::GetCurrentProcessId())
            return true;

        platform_error error;
        return !wait_until(0, error);
    }

    return false;
}

bool process::wait_until(int millisec, platform_error& error)
{
    error.clear();

    if (valid())
    {
        if (id() == ::GetCurrentProcessId())
        {
            error = platform_error(-1, "Can't wait for itself");
            return false;
        }

        // �ú�������handleָ��Ķ��������� SYNCHRONIZE Ȩ��;
        DWORD result = ::WaitForSingleObject(
            (HANDLE)_handle, 
            millisec == -1 ? INFINITE : millisec);

        switch (result)
        {
        case WAIT_OBJECT_0: // ���󱻴���
            return true;
        
        case WAIT_FAILED:
            error = platform_error(::GetLastError(), "Can't wait the process");
            return false;

        case WAIT_TIMEOUT:  // ��ʱ������
        default:
            return false;
        }
    }

    return true;
}

void process::terminate(platform_error& error)
{
    error.clear();

    if (valid() && running())
    {
        // �ú���Ҫ��, handle ָ��ľ��������� PROCESS_TERMINATE Ȩ��
        if (!::TerminateProcess((HANDLE)_handle, -1))
        {
            error = platform_error(::GetLastError(), "Can't terminate the process");
        }
    }
}

} // util