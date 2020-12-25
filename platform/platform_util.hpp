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

/*!
 *  \brief 动态判断当前CPU的字节序
 * 
 *  \note  优先考虑: CPU_LITTLE_ENDIAN
 */
UTILITY_FUNCT_DECL bool is_little_endian();

/*!
 *  \brief 返回特定平台的错误码描述字符串
 */
UTILITY_FUNCT_DECL std::string format_error(int error_code);
UTILITY_FUNCT_DECL std::wstring wformat_error(int error_code);

} // util

#if OS_WIN
#   include <platform/platform_win.hpp>
#else
#   include <platform/platform_unix.hpp>
#endif

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/platform_util.ipp"
#endif

#endif // platform_util_h__
