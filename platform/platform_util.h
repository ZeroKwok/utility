#ifndef platform_util_h__
#define platform_util_h__

/*
*   platform_util.h
*
*   v0.1 2018-03 By GuoJH
*   v0.2 2020-09 By GuoJH
*/

#include <string>
#include <platform/platform_cfg.h>
#include <platform/platform_error.h>

namespace util{

/*!
 *  \brief 动态判断当前CPU的字节序
 * 
 *  \note  优先考虑: CPU_LITTLE_ENDIAN
 */
UTILITY_FUNCT_DECL bool is_little_endian();

/*!
 *  \brief 返回环境变量name的内容
 *  
 *  \param name 环境变量的名称
 *  \note  环境变量不存在或出错则返回空。
 */
UTILITY_FUNCT_DECL std::string environment_variable(const char* name);

/*!
 *  \brief 设置环境变量
 *
 *  \param name  环境变量的名称
 *  \param value 环境变量的内容, 可以为空, 此时将删除现有的环境变量.
 *  \return 如果不能设置环境变量则返回false.
 */
UTILITY_FUNCT_DECL bool set_environment_variable(
    const std::string& name, const std::string& value);

/*!
 *  \brief 返回特定平台的错误码描述字符串
 */
UTILITY_FUNCT_DECL std::string format_error(int error_code);
UTILITY_FUNCT_DECL std::wstring wformat_error(int error_code);

/*!
 *  \brief 格式化输出调试信息
 * 
 *  \note  在程序控制台中打印消息, 另外如果进程被调试, 在Windows中消息会同时输出至调试器.
 */
UTILITY_FUNCT_DECL void output_debug_string(const char* format, ...);
UTILITY_FUNCT_DECL void output_debug_string(const std::string& message);
UTILITY_FUNCT_DECL void output_debug_string(const std::wstring& message);

} // util

#if OS_WIN
#   include <platform/platform_win.h>
#elif OS_POSIX
#   include <platform/platform_unix.h>
#endif

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/platform_util.ipp"
#endif

#endif // platform_util_h__
