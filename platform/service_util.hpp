#ifndef service_util_h__
#define service_util_h__

/*
*   service_util.hpp 
*
*   v0.1 2018-04 By GuoJH
*   v0.2 2020-10 By GuoJH
*/

#include <string/tstring.hpp>
#include <platform/platform_cfg.hpp>

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
#   include "impl/service_util.ipp "
#endif

#endif // service_util_h__
