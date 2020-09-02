#ifndef platform_util_h__
#define platform_util_h__

/*
*   platform_util.hpp 
*
*   v0.1 2018-03 By GuoJH
*   v0.2 2020-09 By GuoJH
*/

#include <string>
#include <platform/platform_cfg.hpp>

namespace util{
namespace win {

// Returns true, if this program running on Wow64.
UTILITY_FUNCT_DECL bool is_wow64();

// Returns true, if this program running on 64bit OS.
UTILITY_FUNCT_DECL bool is_64bitsys();

// Returns true, if this program running on the little-endian CPU.
UTILITY_FUNCT_DECL bool is_little_endian();

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

// Returns the name of the current system
// Note: https://docs.microsoft.com/zh-cn/windows/win32/sysinfo/targeting-your-application-at-windows-8-1
UTILITY_FUNCT_DECL std::string system_name();
UTILITY_FUNCT_DECL std::wstring wsystem_name();

// Returns a string with the meaning of the error_code error code.
UTILITY_FUNCT_DECL std::string format_error(int error_code);
UTILITY_FUNCT_DECL std::wstring wformat_error(int error_code);

} // win
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/platform_util.ipp "
#endif

#endif // platform_util_h__
