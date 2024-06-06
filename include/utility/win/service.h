// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.
//
// v0.1 2018-04
// v0.2 2020-10
// v0.3 2024-06

#ifndef service_win_h__
#define service_win_h__

#include "error.h"
#include <filesystem>
#include <functional>

#ifndef OS_WIN
#   error service.h only supports Windows.
#endif

namespace UTILITY_NAMESPACE {
namespace win {

//! @brief 安装服务到系统
//! @param name 服务名称
//! @param display_name 服务显示的名称
//! @param dependencies 服务依赖项, 可以为空
//! @param account 运行服务的用户账号, 默认为: NT AUTHORITY\LocalService
//! @param password 用户账号的密码，"" 表示空密码，nullptr 表示未指定
//! @param path 服务文件路径
//! @param start_type 启动类型，默认为：SERVICE_DEMAND_START
//! @param error 出错时将存储具体的错误信息.
//! @return 成功返回true, 失败返回false.
UTILITY_API bool service_install(
    const std::wstring& name, 
    const std::wstring& display_name,
    const std::wstringstd::wstring& account,
    const std::optional<std::wstring>& password,
    const std::filesystem::path& path,
    int start_type,
    std::error_code& error);

//! brief 更新指定的服务配置
UTILITY_API bool service_change_config(
    const std::wstring& name, 
    const std::wstring& display_name,
    const std::wstringstd::wstring& account,
    const std::optional<std::wstring>& password,
    const std::filesystem::path& path,
    int start_type,
    std::error_code& error);

//! @brief 移除指定的服务
//! @param name 服务名
//! @param error 出错时将存储具体的错误信息.
//! @return 成功返回true, 否者false.
UTILITY_API bool service_remove(const std::wstring& name, std::error_code& error);

//! @brief 移除服务
//! @param name 服务名
//! @param error 出错时将存储具体的错误信息.
//! @return 成功返回true, 否者false.
UTILITY_API bool service_start(const std::wstring& name, std::error_code& error);

//! @brief 移除指定的服务
//! @param name 服务名
//! @param error 出错时将存储具体的错误信息.
//! @return 成功返回true, 否者false.
UTILITY_API bool service_restart(const std::wstring& name, int interval, std::error_code& error);

//! @brief 停止指定的服务
//! @param name 服务名
//! @param error 出错时将存储具体的错误信息.
//! @return 成功返回true, 否者false.
UTILITY_API bool service_stop(const std::wstring& name, std::error_code& error);

//! @brief 检查服务是否存在
//! @param name 服务名
//! @param error 出错时将存储具体的错误信息.
//! @return 成功返回true, 否者false.
UTILITY_API bool service_exist(const std::wstring& name, std::error_code& error);

//! @brief 检查服务是否正在运行
//! @param name 服务名
//! @param error 出错时将存储具体的错误信息.
//! @return 成功返回true, 否者false.
UTILITY_API bool service_is_running(const std::wstring& name, std::error_code& error);

} // win
} // UTILITY_NAMESPACE

#endif // service_win_h__
