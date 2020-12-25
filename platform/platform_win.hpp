#ifndef platform_win_h__
#define platform_win_h__

/*
*   platform_for_win.hpp 
*
*   v0.1 2018-03 By GuoJH
*   v0.2 2020-12 By GuoJH
*/

#include <utility.hpp>

#if OS_WIN

#include <string>
#include <platform/platform_cfg.hpp>

namespace util{
namespace win {

// Returns true, if this program running on Wow64.
// That is, 32-bit programs run on 64-bit systems.
UTILITY_FUNCT_DECL bool is_wow64();

// Returns true, if this program running on 64bit OS.
UTILITY_FUNCT_DECL bool is_64bitsys();

UTILITY_FUNCT_DECL bool is_running_on_vista_or_higher();
UTILITY_FUNCT_DECL bool is_running_on_win7_or_higher();
UTILITY_FUNCT_DECL bool is_running_on_win7_sp1_or_higher();

// Determine if the user is part of the adminstators group. This will return
// true in case of XP and 2K if the user belongs to admin group. In case of
// Vista, it only returns true if the admin is running elevated.
UTILITY_FUNCT_DECL bool is_user_admin();

// Returns true if the user is running as a non-elevated admin in case of
// Vista. In case of XP always returns false.
UTILITY_FUNCT_DECL bool is_user_non_elevated_admin();

// Returns true if the network is available.
UTILITY_FUNCT_DECL bool is_network_available();

// Sets the name of the given thread.
UTILITY_FUNCT_DECL bool set_thread_name(const std::string& name, int thread_id = -1);

// Returns the name of the current system
// Note: https://docs.microsoft.com/zh-cn/windows/win32/sysinfo/targeting-your-application-at-windows-8-1
UTILITY_FUNCT_DECL std::string system_name();
UTILITY_FUNCT_DECL std::wstring wsystem_name();

// Returns a string with the meaning of the error_code error code.
UTILITY_FUNCT_DECL std::string format_error(int error_code);
UTILITY_FUNCT_DECL std::wstring wformat_error(int error_code);

// Returns true if the error_code is the network error.
UTILITY_FUNCT_DECL bool is_network_error(int error_code);

} // win
} // util

#endif // OS_WIN

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/platform_win.ipp "
#endif

#endif // platform_win_h__
