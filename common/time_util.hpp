#ifndef time_util_h__
#define time_util_h__

/*
*   time_util.hpp 
*
*   v0.1 2018-06 by GuoJH
*/

#include <string>
#include <common/common_cfg.hpp>

namespace util {

/*!
 *	����ϵͳ�������������Ժ���Ƶ�ʱ��, ������Ƶ���Ա�ʱ��ĳ���;
 */
UTILITY_FUNCT_DECL int64_t time_ticks_now();

/*!
 *	ʱ����Ϣ
 *  ��Ӧ�˵�ǰϵͳ����ʱ����UTCʱ��Ĳ��;
 */
struct timezone_t
{
    int hours;
    int minutes;
    int duration;
};

/*!
 *	��õ�ǰϵͳ��ʱ����Ϣ;
 */
UTILITY_FUNCT_DECL void timezone_information(timezone_t& zone);

/*!
 *	����GMTʱ��
 *  2018-08-22 09:45:54 +0800
 */
UTILITY_FUNCT_DECL std::string time_gmt();
UTILITY_FUNCT_DECL std::wstring wtime_gmt();

/*!
 *	��ʽ��Uinxʱ���
 *  time ����Ϊ����, �����ܸ�ʽ��epoch��Ԫ֮ǰ��ʱ��;
 */
UTILITY_FUNCT_DECL std::string format_epoch(
    time_t time, const char* format = "%d-%m-%Y %H:%M, %a");
UTILITY_FUNCT_DECL std::wstring wformat_epoch(
    time_t time, const wchar_t* format = L"%d-%m-%Y %H:%M, %a");

/*!
 *  �ļ�ʱ��ת��epoch, ��λ΢��
 */
#ifdef OS_WIN
UTILITY_FUNCT_DECL int64_t epoch_from_filetime(FILETIME ft);
#endif

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/time_util.ipp"
#endif

#endif // time_util_h__
