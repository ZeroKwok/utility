#ifndef platform_unix_h__
#define platform_unix_h__

/*
*   platform_unix.hpp 
*
*   v0.1 2020-12 By GuoJH
*/

#include <utility.hpp>

#if OS_POSIX

#include <string>
#include <platform/platform_cfg.hpp>

namespace util{
namespace posix{

//! 返回指定错误码error_code的描述信息.
UTILITY_FUNCT_DECL std::string format_error(int error_code);
UTILITY_FUNCT_DECL std::wstring wformat_error(int error_code);

} // posix
} // util

#endif // OS_POSIX

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/platform_unix.ipp"
#endif

#endif // platform_unix_h__
