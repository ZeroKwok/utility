#ifndef unit_h__
#define unit_h__

/*
*   unit.h
*
*   v0.1 2018-10 by GuoJH
*/

#include <string>
#include <common/common_cfg.h>

namespace util {

/*!
 *   转换字节数到合适的单位并添加后缀
 */
UTILITY_FUNCT_DECL std::string bytes_add_suffix(
    double bytes, int base = 1024, const std::string& suffix = "");
UTILITY_FUNCT_DECL std::wstring wbytes_add_suffix(
    double bytes, int base = 1024, const std::wstring& suffix = L"");

/*!
 *   转换秒数到合适的单位并添加后缀
 */
UTILITY_FUNCT_DECL std::string seconds_add_suffix(
    int seconds, const std::string& suffix = "");
UTILITY_FUNCT_DECL std::wstring wseconds_add_suffix(
    int seconds, const std::wstring& suffix = L"");

/*!
 *   以指定的时长与分隔符转换到合适的单位
 */
UTILITY_FUNCT_DECL std::string duration_format(
    int seconds, const std::string& separator = ":");
UTILITY_FUNCT_DECL std::wstring wduration_format(
    int seconds, const std::wstring& separator = L":");

UTILITY_FUNCT_DECL std::string duration_ms_format(
    int milliseconds, const std::string& separator = ":");
UTILITY_FUNCT_DECL std::wstring wduration_ms_format(
    int milliseconds, const std::wstring& separator = L":");

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/unit.ipp"
#endif

#endif // unit_h__
