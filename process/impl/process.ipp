#ifdef UTILITY_DISABLE_HEADONLY
#   include "../process.hpp"
#endif

#include <windows.h>
#include <shellapi.h>
#include <string/string_util.hpp>
#include <platform/platform_util.hpp>

namespace util {
namespace detail {
  
// 直接运行
inline HANDLE runapp_with_redirection(
    const util::tstring& app,
    const util::tstring& cmd,
    HANDLE input,
    HANDLE output,
    HANDLE error,
    process::launch_policy policy,
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

    std::wstring cmd_dup(cmd);

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

// 通过shell运行, 主要用于提权
inline HANDLE runapp_shell_and_elevate_privileges(
    util::tstring app,
    util::tstring cmd,
    process::launch_policy policy,
    platform_error& error)
{
    /*
    *   ShellExecuteExW与CreateProcessW不同的是lpFile字段必须有值
    *   不能在参数lpParameters中指定可执行模块
    *   这里为了兼容RunAppWithRedirection, 需要手动分割混在cmd中的
    *   模块名
    *   2018-4-12 By GuoJH
    */

    if (app.empty() && !cmd.empty())
    {
        /* 开始包含双引号, 说明可执行模块由引号分割, 否则是以空格分割 */

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
    exec_info.fMask         = SEE_MASK_NOCLOSEPROCESS;  // 不关闭进程句柄
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

// 以非特权的方式运行, 主要用于降低权限
inline HANDLE runapp_with_non_elevate_privileges(
    const util::tstring& app,
    const util::tstring& cmd,
    HANDLE input,
    HANDLE output,
    HANDLE error,
    process::launch_policy policy,
    platform_error& perror)
{ 
    // vista 及以下, 或者 非提升的管理员
    if(!win::is_running_on_vista_or_higher() || win::is_user_non_elevated_admin())
    {
        return runapp_with_redirection(app, cmd, input, output, error, policy, perror);
    }

    // 通过动态加载的方式使用该函数, 目的在于避免vista以下没有该函数的机器上
    // 能正确的启动程序;
    typedef BOOL (*func_CreateProcessWithTokenW)(
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

    std::wstring cmd_dup(cmd);

    // 获得explorer进程Pid
    DWORD explorerPid = 0;

    ::GetWindowThreadProcessId(::GetShellWindow(), &explorerPid);

    // ATTENTION:
    // If UAC is turned OFF all processes run with SECURITY_MANDATORY_HIGH_RID, also Explorer!
    // But this does not matter because to start the new process without UAC no elevation is required.
    HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, explorerPid);

    if(!hProcess)
        goto _CleanUp;

    // 打开进程token, 并复制它
    HANDLE hToken = 0;
    HANDLE hToken2 = 0;
    if(!::OpenProcessToken(hProcess, TOKEN_DUPLICATE, &hToken))
        goto _CleanUp;

    if (!::DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, 0, SecurityImpersonation, TokenPrimary, &hToken2))
        goto _CleanUp;

    // 通过复制的令牌启动进程
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

    // 检查错误
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
    platform_error& error,
    launch_policy policy/* = defaulted*/)
{
    error.clear();

    if (policy & elevate_privileges && win::is_user_admin())
        policy = launch_policy(policy & (~elevate_privileges));

    if (policy & depress_privileges && win::is_user_non_elevated_admin())
        policy = launch_policy(policy & (~depress_privileges));

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
        std::terminate();
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
    return _handle;
}

int process::exit_code() const
{
    if (valid())
    {
        DWORD code = 0;
        if (GetExitCodeProcess((HANDLE)_handle, &code))
            return code;
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
        wait_until(-1);
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
        return !wait_until(0);

    return false;
}

bool process::wait_until(int millisec/* = -1*/)
{
    if (valid())
    {
        return ::WaitForSingleObject(
            (HANDLE)_handle,
            millisec == -1 ? INFINITE : millisec) == WAIT_OBJECT_0;
    }

    return true;
}

void process::terminate(platform_error& error)
{
    error.clear();

    if (valid())
    {
        if (!::TerminateProcess((HANDLE)_handle, 0))
        {
            error = platform_error(::GetLastError(), "Can't terminate the process");
        }
    }
}

} // util