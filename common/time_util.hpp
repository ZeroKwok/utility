#ifndef time_util_h__
#define time_util_h__

/*
*   time_util.hpp 
*
*   v0.1 2018-06 by GuoJH
*/

#ifdef OS_WIN
#   include <windows.h>
#endif

#include <string>
#include <common/common_cfg.hpp>

namespace util {

/*!
 *  \brief 时区信息
 *         反应了当前系统本地时间与UTC时间的差距;
 */
struct timezone_t
{
    int hours;
    int minutes;
    int duration;
};

/*!
 *  \brief 获得当前系统的时区信息;
 */
UTILITY_FUNCT_DECL void timezone_information(timezone_t& zone);

/*!
 *  \brief 返回GMT时间
 *         如: 2018-08-22 09:45:54 +0800
 */
UTILITY_FUNCT_DECL std::string time_gmt();
UTILITY_FUNCT_DECL std::wstring wtime_gmt();

/*!
 *  \brief 格式化Uinx时间戳
 *  
 *  \note  time 不能为负数, 即不能格式化epoch纪元之前的时间;
 */
UTILITY_FUNCT_DECL std::string format_epoch(
    time_t time, const char* format = "%d-%m-%Y %H:%M, %a");
UTILITY_FUNCT_DECL std::wstring wformat_epoch(
    time_t time, const wchar_t* format = L"%d-%m-%Y %H:%M, %a");

#ifdef OS_WIN

/*!
 *  \brief 返回系统自启动以来的以毫秒计的时间
 *         适用于频繁对比时间的场合, 仅支持Windows平台.
 * 
 *  \note  弃用, 考虑chrono代替.
 */
UTILITY_FUNCT_DECL int64_t time_ticks_now();

/*!
 *  \brief 文件时间转换epoch, 单位微秒
 *         仅支持Windows平台
 */
UTILITY_FUNCT_DECL int64_t epoch_from_filetime(FILETIME ft);

#endif // OS_WIN

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/time_util.ipp"
#endif

#endif // time_util_h__
