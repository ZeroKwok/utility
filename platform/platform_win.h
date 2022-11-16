#ifndef platform_win_h__
#define platform_win_h__

/*
*   platform_for_win.h
*
*   v0.1 2018-03 By GuoJH
*   v0.2 2020-12 By GuoJH
*/

#ifndef platform_util_h__
#   include <platform/platform_util.h>
#endif

#include <list>
#include <string>
#include <windows.h>

namespace util{
namespace win {

//! Returns true, if this program running on Wow64.
//! That is, 32-bit programs run on 64-bit systems.
UTILITY_FUNCT_DECL bool is_wow64();

//! Returns true, if this program running on 64bit OS.
UTILITY_FUNCT_DECL bool is_64bitsys();

UTILITY_FUNCT_DECL bool is_running_on_vista_or_higher();
UTILITY_FUNCT_DECL bool is_running_on_win7_or_higher();
UTILITY_FUNCT_DECL bool is_running_on_win7_sp1_or_higher();
UTILITY_FUNCT_DECL bool is_running_on_win11_or_higher();

//! Determine if the user is part of the adminstators group. This will return
//! true in case of XP and 2K if the user belongs to admin group. In case of
//! Vista, it only returns true if the admin is running elevated.
UTILITY_FUNCT_DECL bool is_user_admin();

//! Returns true if the user is running as a non-elevated admin in case of
//! Vista. In case of XP always returns false.
UTILITY_FUNCT_DECL bool is_user_non_elevated_admin();

//! Returns true if the network is available.
//! There may be several hundred milliseconds of consumption.
UTILITY_FUNCT_DECL bool is_network_available();

//! Returns the name of the current system
//! Note: https://!docs.microsoft.com/zh-cn/windows/win32/sysinfo/targeting-your-application-at-windows-8-1
UTILITY_FUNCT_DECL std::string system_name();
UTILITY_FUNCT_DECL std::wstring wsystem_name();

//! 返回系统真实的版本号, 是::GetVersionExW()系统API的包装
//! 
//! GetVersionExW()只支持到Win8.1, 此后该方法获得的版本号最大仅为Win 8.1.
//! 
//! Applications not manifested for Windows 8.1 or Windows 10 will return the Windows 8 OS version value (6.2). 
//! Once an application is manifested for a given operating system version, 
//! GetVersionEx will always return the version that the application is manifested for in future releases. 
//! https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getversionexw
//!
UTILITY_FUNCT_DECL OSVERSIONINFOEXW get_osversionex_w();

//! Returns a string with the meaning of the error_code error code.
UTILITY_FUNCT_DECL std::string format_error(int error_code);
UTILITY_FUNCT_DECL std::wstring wformat_error(int error_code);

//! Returns true if the error_code is a common network error.
UTILITY_FUNCT_DECL bool is_network_error(int error_code);

//! Sets the name of the given thread.
UTILITY_FUNCT_DECL bool set_thread_name(const std::string& name, int thread_id = -1);

//! stack frame
struct stack_frame_t
{
    std::wstring module_name;
    std::wstring symbol_name;
    uint64_t     symbol_address;
    uint32_t     line_number;
    std::wstring file_name;
};

typedef std::list<stack_frame_t> call_stack_t;

//! 返回是否支持栈回溯
UTILITY_FUNCT_DECL bool is_supported_stack_trace();

//! Query the stack frame
UTILITY_FUNCT_DECL bool get_stack_frame(call_stack_t& frames, int skip = 0, int capture = 5);

//! Format the stack frame into a string
UTILITY_FUNCT_DECL std::wstring& format_stack_trace(
    const call_stack_t& frames, std::wstring& trace);

//! Query and Format stack frame into a string
UTILITY_FUNCT_DECL std::wstring stack_trace(int skip = 0, int capture = 5);

} // win
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/platform_win.ipp "
#endif

#endif // platform_win_h__
