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

//! @brief 安装服务到系统
//! @param name 服务名称
//! @param display_name 服务显示的名称
//! @param path 服务文件路径
//! @param dependencies 服务依赖项, 可以为空
//! @param account 运行的账号名, 默认为: NT AUTHORITY\\LocalService
//! @param password account指向账户的密码
//! @param start_type 
//! @param error 出错时将存储具体的错误信息.
//! @return 成功返回true, 失败返回false.
bool service_install(
    const util::tstring& name, 
    const util::tstring& display_name,
    const util::tstring& path,
    const util::tstring& dependencies/* = L""*/,
    const util::tstring& account     /* = L"NT AUTHORITY\\LocalService"*/,
    wchar_t*             password    /* = nullptr*/,
    int                  start_type  /* = SERVICE_DEMAND_START*/,
    platform_error&      error);

//! 更新指定的服务配置
bool service_change_config(
    const util::tstring& name, 
    const util::tstring& display_name,
    const util::tstring& path,
    const util::tstring& account,
    wchar_t*             password,
    int                  start_type,
    platform_error&      error);

//! @brief 移除指定的服务
//! @param name 服务名
//! @param error 出错时将存储具体的错误信息.
//! @return 成功返回true, 否者false.
bool service_remove(
    const util::tstring& name, platform_error& error);

//! @brief 移除服务
//! @param name 服务名
//! @param error 出错时将存储具体的错误信息.
//! @return 成功返回true, 否者false.
bool service_start(
    const util::tstring& name, platform_error& error);

//! @brief 移除指定的服务
//! @param name 服务名
//! @param error 出错时将存储具体的错误信息.
//! @return 成功返回true, 否者false.
bool service_restart(
    const util::tstring& name, int interval/* = 500*/, platform_error& error);

//! @brief 停止指定的服务
//! @param name 服务名
//! @param error 出错时将存储具体的错误信息.
//! @return 成功返回true, 否者false.
bool service_stop(
    const util::tstring& name, platform_error& error);

//! @brief 检查服务是否存在
//! @param name 服务名
//! @param error 出错时将存储具体的错误信息.
//! @return 成功返回true, 否者false.
bool service_exist(
    const util::tstring& name, platform_error& error);

//! @brief 检查服务是否正在运行
//! @param name 服务名
//! @param error 出错时将存储具体的错误信息.
//! @return 成功返回true, 否者false.
bool service_running(
    const util::tstring& name, platform_error& error);

} // win
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/service_win.ipp "
#endif

#endif // service_win_h__
