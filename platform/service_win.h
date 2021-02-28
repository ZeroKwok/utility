#ifndef service_win_h__
#define service_win_h__

/*
*   service_win.h 
*
*   v0.1 2018-04 By GuoJH
*   v0.2 2020-10 By GuoJH
*/

#include <utility.hpp>

#ifndef OS_WIN
#   error service_win.h only supports Windows.
#endif

#include <string/tstring.h>
#include <platform/platform_cfg.h>
#include <platform/platform_error.h>

namespace util{
namespace win {

/*!
 *  /brief  安装windows服务
 */
bool service_install(
    const util::tstring& name, 
    const util::tstring& display_name,
    const util::tstring& path,
    const util::tstring& dependencies/* = L""*/,
    const util::tstring& account     /* = L"NT AUTHORITY\\LocalService"*/,
    wchar_t*             password    /* = nullptr*/,
    int                  start_type  /* = SERVICE_DEMAND_START*/,
    platform_error&      error);

bool service_change_config(
    const util::tstring& name, 
    const util::tstring& display_name,
    const util::tstring& path,
    const util::tstring& account,
    wchar_t*             password,
    int                  start_type,
    platform_error&      error);

bool service_remove(
    const util::tstring& name, platform_error& error);

bool service_start(
    const util::tstring& name, platform_error& error);

bool service_restart(
    const util::tstring& name, int interval/* = 500*/, platform_error& error);

bool service_stop(
    const util::tstring& name, platform_error& error);

bool service_exist(
    const util::tstring& name, platform_error& error);

bool service_running(
    const util::tstring& name, platform_error& error);

} // win
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/service_win.ipp "
#endif

#endif // service_win_h__
