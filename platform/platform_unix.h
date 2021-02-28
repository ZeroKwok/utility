#ifndef platform_unix_h__
#define platform_unix_h__

/*
*   platform_unix.h
*
*   v0.1 2020-12 By GuoJH
*/

#ifndef platform_util_h__
#   include <platform/platform_util.h>
#endif

namespace util{
namespace posix{

//! 返回指定错误码error_code的描述信息.
UTILITY_FUNCT_DECL std::string format_error(int error_code);
UTILITY_FUNCT_DECL std::wstring wformat_error(int error_code);

} // posix
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/platform_unix.ipp"
#endif

#endif // platform_unix_h__
