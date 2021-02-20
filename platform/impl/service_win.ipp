#ifdef UTILITY_DISABLE_HEADONLY
#   include "../service_win.hpp"
#endif

#include <windows.h>
#include <platform/platform_error.hpp>

namespace util{
namespace win {
namespace detail {

/*
*    以指定权限打开服务控制管理器(service control manager)
*   SC_MANAGER_ALL_ACCESS   所有权限
*/
inline SC_HANDLE InitSCManager(DWORD dwAccess, platform_error& error)
{
    // SCM设置的错误, 其他错误由调用的注册表函数
    // ERROR_ACCESS_DENIED
    // ERROR_DATABASE_DOES_NOT_EXIST

    SC_HANDLE hSCManager = ::OpenSCManager(
        NULL,                    // local computer
        NULL,                    // servicesActive database 
        dwAccess);               // full access rights 

    if (hSCManager == nullptr)
    {
        error = platform_error(::GetLastError(), "Can't open service control manager");
    }

    return hSCManager;
}

/*
*    以指定的服务名称及权限打开服务
*   SERVICE_ALL_ACCESS  所有权限
*/
inline SC_HANDLE OpenServiceWithName(
    SC_HANDLE hSCManager, const std::wstring& name, DWORD dwAccess, platform_error& error)
{
    // ERROR_ACCESS_DENIED          The handle does not have access to the service.
    // ERROR_INVALID_HANDLE         The specified handle is invalid.
    // ERROR_INVALID_NAME           The specified service name is invalid.
    // ERROR_SERVICE_DOES_NOT_EXIST The specified service does not exist.

    SC_HANDLE hService =  ::OpenServiceW(
        hSCManager,         // SCM database 
        name.c_str(),       // name of service 
        dwAccess);          // full access 

    if (hService == nullptr)
    {
        error = platform_error(::GetLastError(), "Can't open service with service name");
    }

    return hService;
}

/*
*    查询服务状态
*/
inline bool QueryServiceStatusWithHandle(
    SC_HANDLE hService, SERVICE_STATUS_PROCESS& status, platform_error& error)
{
    DWORD dwBytesNeeded = 0;

    // Check the status in case the service is not stopped. 
    if (::QueryServiceStatusEx( 
        hService,                           // handle to service 
        SC_STATUS_PROCESS_INFO,             // information level
        (LPBYTE) &status,                   // address of structure
        sizeof(SERVICE_STATUS_PROCESS),     // size of structure
        &dwBytesNeeded)                     // size needed if buffer is too small
        == 0)                  
    {
        error = platform_error(::GetLastError(), "Can't query service status with service handle");
        return false;
    }

    return true;
}

/*
*    等待服务从某个状态过渡某个状态
*/
inline bool WaitServiceChangeToSomeState(
    SC_HANDLE hService, SERVICE_STATUS_PROCESS& status, DWORD state, platform_error& error)
{
    DWORD dwOldCheckPoint   = status.dwCheckPoint;
    DWORD dwStartTickCount  = ::GetTickCount();
    DWORD dwWaitTime;

    // Wait for the service to stop before attempting to start it.
    while (status.dwCurrentState == state)
    {
        // Do not wait longer than the wait hint. A good interval is 
        // one-tenth of the wait hint but not less than 1 second  
        // and not more than 10 seconds. 
        dwWaitTime = status.dwWaitHint / 10;

        if( dwWaitTime < 1000 )
            dwWaitTime = 1000;
        else if ( dwWaitTime > 10000 )
            dwWaitTime = 10000;

        ::Sleep( dwWaitTime );

        // Check the status until the service is no longer stop pending. 
        if( !QueryServiceStatusWithHandle(hService, status, error) )
            return false;

        if ( status.dwCheckPoint > dwOldCheckPoint )
        {
            // Continue to wait and check.
            dwStartTickCount = ::GetTickCount();
            dwOldCheckPoint = status.dwCheckPoint;
        }
        else
        {
            if(::GetTickCount()-dwStartTickCount > status.dwWaitHint)
            {
                // No progress made within the wait hint.
                error = platform_error(-1, "Waiting for a service state change to time out");
                return false;
            }
        }
    }

    return true;
}

/*
*    启动服务
*/
inline bool StartServiceWithHandle(SC_HANDLE hService, platform_error& error)
{
    SERVICE_STATUS_PROCESS status = {0};

    if( !QueryServiceStatusWithHandle(hService, status, error) )
        return false;

    // Check if the service is already running. It would be possible 
    // to stop the service here, but for simplicity this example just returns. 
    if(status.dwCurrentState != SERVICE_STOPPED && status.dwCurrentState != SERVICE_STOP_PENDING)
        return true; 


    DWORD dwOldCheckPoint    = status.dwCheckPoint;
    DWORD dwStartTickCount   = ::GetTickCount();

    // Wait for the service to stop before attempting to start it.
    if (!WaitServiceChangeToSomeState(hService, status, SERVICE_STOP_PENDING, error))
        return false;
 
    // Attempt to start the service.
    if (::StartServiceW(
        hService,       // handle to service 
        0,              // number of arguments 
        NULL) == 0)     // no arguments 
    {
        error = platform_error(::GetLastError(), "Start the Service failed with the service handle");
        return false; 
    }

    if( !QueryServiceStatusWithHandle(hService, status, error) )
        return false;

    WaitServiceChangeToSomeState(hService, status, SERVICE_START_PENDING, error);

    return status.dwCurrentState == SERVICE_RUNNING;
}

// 用于规避__try的错误: error C2712:  __try
struct error_struct
{
    int         code;
    const char* message;
};

/*
*    停止所有此服务的依赖
*/
inline bool _StopServiceAllDependent(SC_HANDLE hSCManager, SC_HANDLE hService, error_struct& error)
{
    DWORD i;
    DWORD dwBytesNeeded;
    DWORD dwCount;

    LPENUM_SERVICE_STATUSW  lpDependencies = NULL;
    ENUM_SERVICE_STATUSW    ess;
    SC_HANDLE               hDepService;
    SERVICE_STATUS_PROCESS  ssp;

    DWORD dwStartTime = ::GetTickCount();
    DWORD dwTimeout = 30000; // 30-second time-out

    // Pass a zero-length buffer to get the required buffer size.
    if ( ::EnumDependentServicesW(hService, SERVICE_ACTIVE,
        lpDependencies, 0, &dwBytesNeeded, &dwCount ) ) 
    {
        // If the Enum call succeeds, then there are no dependent
        // services, so do nothing.
        return TRUE;
    } 
    else 
    {
        if (GetLastError() != ERROR_MORE_DATA)
        {
            error.code = GetLastError();
            error.message = "Can't stop all dependencies on the service";
            return FALSE; 
        }

        // Allocate a buffer for the dependencies.
        lpDependencies = (LPENUM_SERVICE_STATUSW) ::HeapAlloc( 
            ::GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded );

        if ( !lpDependencies )
        {
            error.code = GetLastError();
            error.message = "Can't Allocate a buffer for the dependencies";
            return FALSE;
        }

        __try 
        {
            // Enumerate the dependencies.
            if (!::EnumDependentServicesW(hService, SERVICE_ACTIVE,
                lpDependencies, dwBytesNeeded, &dwBytesNeeded,
                &dwCount))
            {
                error.code = GetLastError();
                error.message = "Can't enumerate all the dependencies of a service";
                return FALSE;
            }

            for ( i = 0; i < dwCount; i++ ) 
            {
                ess = *(lpDependencies + i);

                // Open the service.
                hDepService = ::OpenServiceW(
                    hSCManager,                             // SCM database 
                    ess.lpServiceName,                      // name of service 
                    SERVICE_STOP | SERVICE_QUERY_STATUS);   // full access 

                if (hDepService == nullptr)
                {
                    error.code = GetLastError();
                    error.message = "Can't open service with service name";
                    return FALSE;
                }

                __try 
                {
                    // Send a stop code.
                    if (!::ControlService(hDepService,
                        SERVICE_CONTROL_STOP,
                        (LPSERVICE_STATUS)&ssp))
                    {
                        error.code = GetLastError();
                        error.message = "Send a stop code to service failed";
                        return FALSE;
                    }

                    // Wait for the service to stop.
                    while ( ssp.dwCurrentState != SERVICE_STOPPED ) 
                    {
                        ::Sleep( ssp.dwWaitHint );

                        if ( !::QueryServiceStatusEx( 
                            hDepService, 
                            SC_STATUS_PROCESS_INFO,
                            (LPBYTE)&ssp, 
                            sizeof(SERVICE_STATUS_PROCESS),
                            &dwBytesNeeded ) )
                        {
                            error.code = GetLastError();
                            error.message = "Can't query service status with service handle";
                            return FALSE;
                        }

                        if ( ssp.dwCurrentState == SERVICE_STOPPED )
                            break;

                        if ( ::GetTickCount() - dwStartTime > dwTimeout )
                        {
                            error.code = GetLastError();
                            error.message = "Waiting for a service state change to time out";
                            return FALSE;
                        }
                    }
                } 
                __finally 
                {
                    // Always release the service handle.
                    ::CloseServiceHandle( hDepService );
                }
            }
        } 
        __finally 
        {
            // Always free the enumeration buffer.
            HeapFree( ::GetProcessHeap(), 0, lpDependencies );
        }
    } 

    return TRUE;
}

inline bool StopServiceAllDependent(SC_HANDLE hSCManager, SC_HANDLE hService, platform_error& error)
{
    error_struct _error = {0};
    if (!_StopServiceAllDependent(hSCManager, hService, _error))
    {
        error = platform_error(_error.code, _error.message);
        return false;
    }

    return true;
}

/*
*    停止服务
*/
inline bool StopServiceeWithHandle(
    SC_HANDLE hSCManager, SC_HANDLE hService, platform_error& error)
{
    DWORD dwStartTime = ::GetTickCount();
    DWORD dwTimeout   = 15000; // 15-second time-out

    // Make sure the service is not already stopped.
    SERVICE_STATUS_PROCESS status;
    if( !QueryServiceStatusWithHandle(hService, status, error) )
        return false;

    // Service is already stopped
    if ( status.dwCurrentState == SERVICE_STOPPED )
        return true;

    // If a stop is pending, wait for it.
    if(status.dwCurrentState == SERVICE_STOP_PENDING )
    {
        if( WaitServiceChangeToSomeState(hService, status, SERVICE_STOP_PENDING, error) )
            return true;
        else
            return false;
    }

    // If the service is running, dependencies must be stopped first.
    StopServiceAllDependent(hSCManager, hService, error);

    // Send a stop code to the service.
    if ( !::ControlService( 
        hService, 
        SERVICE_CONTROL_STOP, 
        (LPSERVICE_STATUS) &status ) )
    {
        error = platform_error(::GetLastError(), "Send a stop code to service failed");
        return false;
    }
       
    // Wait for the service to stop.
    while ( status.dwCurrentState != SERVICE_STOPPED ) 
    {
        ::Sleep( 1000 ); // ssStatus.dwWaitHint

        if( !QueryServiceStatusWithHandle(hService, status, error) )
            return false;

        if ( status.dwCurrentState == SERVICE_STOPPED )
            return true;

        if( ::GetTickCount() - dwStartTime > dwTimeout )
        {
            error = platform_error(-1, "Waiting for a service state change to time out");
            return false;
        }

    }

    return status.dwCurrentState == SERVICE_STOPPED;
}

class AutoSCHandle
{
    AutoSCHandle(const AutoSCHandle&);
    AutoSCHandle& operator=(const AutoSCHandle&);

    SC_HANDLE _hold;
public:
    AutoSCHandle(SC_HANDLE handle)
        : _hold(handle)
    {}

    operator SC_HANDLE() { return _hold; }

    ~AutoSCHandle()
    {
        if (_hold != nullptr)
            ::CloseServiceHandle(_hold);
    }
};

} // detail

bool service_install(
    const util::tstring& name, 
    const util::tstring& display_name,
    const util::tstring& path,
    const util::tstring& dependencies/* = L""*/,
    const util::tstring& account     /* = L"NT AUTHORITY\\LocalService"*/,
    wchar_t*             password    /* = nullptr*/,
    int                  start_type  /* = SERVICE_DEMAND_START*/,
    platform_error&      error)
{
    if (detail::AutoSCHandle hSCMAnager = detail::InitSCManager(
        SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE, error))
    {
        // Install the service into SCM by calling CreateService
        detail::AutoSCHandle hService = ::CreateServiceW(
            hSCMAnager,                  // SCManager database
            name.data(),                 // Name of service
            display_name.data(),         // Name to display
            SERVICE_QUERY_STATUS,        // Desired access
            SERVICE_WIN32_OWN_PROCESS,   // Service type
            start_type,                  // Service start type
            SERVICE_ERROR_NORMAL,        // Error control type
            path.data(),                 // Service's binary
            NULL,                        // No load ordering group
            NULL,                        // No tag identifier
            dependencies.data(),         // Dependencies
            account.data(),              // Service running account
            password                     // Password of the account
        );

        if (!hService)
        {
            error = platform_error(GetLastError(), "Can't Create the service");

            return false;
        }

        return true;
    }

    return false;
}

bool service_change_config(
    const util::tstring& name, 
    const util::tstring& display_name,
    const util::tstring& path,
    const util::tstring& account,
    wchar_t*             password,
    int                  start_type,
    platform_error&      error)
{
    if (detail::AutoSCHandle hSCMAnager = detail::InitSCManager(
        SC_MANAGER_ALL_ACCESS, error))
    {
        if (detail::AutoSCHandle hService = detail::OpenServiceWithName(
            hSCMAnager, name, SERVICE_CHANGE_CONFIG, error))
        {
            if (!::ChangeServiceConfigW(
                hService,                                                   // handle of service 
                SERVICE_NO_CHANGE,                                          // service type
                start_type  ,                                               // service start type 
                SERVICE_NO_CHANGE,                                          // error control
                path.empty() ? NULL : path.c_str(),                         // binary path
                NULL,                                                       // load order group
                NULL,                                                       // tag ID
                NULL,                                                       // dependencies
                account.empty() ? NULL : account.c_str(),                   // account name
                password,                                                   // password
                display_name.empty() ? NULL : display_name.c_str()))        // display name
            {
                error = platform_error(GetLastError(), "Can't change the service config");
                return false;
            }

            return true;
        }
    }

    return false;
}

bool service_remove(
    const util::tstring& name, platform_error& error)
{
    if (detail::AutoSCHandle hSCMAnager = detail::InitSCManager(
        SC_MANAGER_CONNECT, error))
    {
        if (detail::AutoSCHandle hService = detail::OpenServiceWithName(
            hSCMAnager, name, SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE, error))
        {
            if (::DeleteService(hService))
            {
                error = platform_error(GetLastError(), "Can't delete the service");

                return false;
            }

            return true;
        }
    }

    return false;
}

bool service_start(
    const util::tstring& name, platform_error& error)
{
    if (detail::AutoSCHandle hSCMAnager = detail::InitSCManager(
        SC_MANAGER_ALL_ACCESS, error))
    {
        if (detail::AutoSCHandle hService = detail::OpenServiceWithName(
            hSCMAnager, name, SERVICE_ALL_ACCESS, error))
        {
            return detail::StartServiceWithHandle(hService, error);
        }
    }

    return false;
}

bool service_restart(
    const util::tstring& name, int interval/* = 500*/, platform_error& error)
{
    if (detail::AutoSCHandle hSCMAnager = detail::InitSCManager(
        SC_MANAGER_ALL_ACCESS, error))
    {
        if (detail::AutoSCHandle hService = detail::OpenServiceWithName(
            hSCMAnager, name, SERVICE_ALL_ACCESS, error))
        {
            SERVICE_STATUS_PROCESS status = {0};
            if (!detail::QueryServiceStatusWithHandle(hService, status, error))
                return false;

            if (status.dwCurrentState == SERVICE_RUNNING ||
                status.dwCurrentState == SERVICE_START_PENDING)
            {
                if (!detail::StopServiceeWithHandle(hSCMAnager, hService, error))
                    return false;

                ::Sleep(interval);
            }

            return detail::StartServiceWithHandle(hService, error);
        }
    }

    return false;
}

bool service_stop(
    const util::tstring& name, platform_error& error)
{
    if (detail::AutoSCHandle hSCMAnager = detail::InitSCManager(
        SC_MANAGER_ALL_ACCESS, error))
    {
        if (detail::AutoSCHandle hService = detail::OpenServiceWithName(
            hSCMAnager, name, SERVICE_ALL_ACCESS, error))
        {
            return detail::StopServiceeWithHandle(hSCMAnager, hService, error);
        }
    }

    return false;
}

bool service_exist(
    const util::tstring& name, platform_error& error)
{
    if (detail::AutoSCHandle hSCMAnager = detail::InitSCManager(
        SC_MANAGER_ALL_ACCESS, error))
    {
        if (detail::AutoSCHandle hService = detail::OpenServiceWithName(
            hSCMAnager, name, SERVICE_ALL_ACCESS, error))
        {
            return true;
        }
    }
    return false;
}

bool service_running(
    const util::tstring& name, platform_error& error)
{
    if (detail::AutoSCHandle hSCMAnager = detail::InitSCManager(
        SC_MANAGER_ALL_ACCESS, error))
    {
        if (detail::AutoSCHandle hService = detail::OpenServiceWithName(
            hSCMAnager, name, SERVICE_ALL_ACCESS, error))
        {
            SERVICE_STATUS_PROCESS status = { 0 };
            if (!detail::QueryServiceStatusWithHandle(hService, status, error))
                return false;

            return status.dwCurrentState == SERVICE_RUNNING ||
                   status.dwCurrentState == SERVICE_START_PENDING;
        }
    }

    return false;
}

} // win
} // util
