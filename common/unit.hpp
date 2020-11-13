#ifndef unit_h__
#define unit_h__

/*
*   unit.hpp
*
*   v0.1 2018-10 by GuoJH
*/

#include <string>
#include <common/common_cfg.hpp>

namespace util {

/*!
 *	ת���ֽ��������ʵĵ�λ, ������±�
 */
UTILITY_FUNCT_DECL std::string bytes_add_suffix(
    double bytes, int base = 1024, const std::string suffix = "");
UTILITY_FUNCT_DECL std::wstring wbytes_add_suffix(
    double bytes, int base = 1024, const std::wstring suffix = L"");

/*!
 *   ת�����������ʵĵ�λ, ������±�
 */
UTILITY_FUNCT_DECL std::string seconds_add_suffix(
    int sec, const std::string suffix = "");
UTILITY_FUNCT_DECL std::wstring wseconds_add_suffix(
    int sec, const std::wstring suffix = L"");

/*!
 *	ת��ʱ�������ʵĵ�λ, ������±�
 */
UTILITY_FUNCT_DECL std::string duration_format(
    int sec, const std::string separator = ":");
UTILITY_FUNCT_DECL std::wstring wduration_format(
    int sec, const std::wstring separator = L":");

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/unit.ipp"
#endif

#endif // unit_h__
