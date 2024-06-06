// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.
//
// v0.1 2019-02
// v0.2 2024-06

#ifndef __dump_win_h
#define __dump_win_h

#include "config.h"
#include <filesystem>
#include <functional>
#ifndef OS_WIN
#   error dump.h only supports Windows.
#endif

namespace UTILITY_NAMESPACE {
namespace win {

class UTILITY_API dump
{
    dump();
    dump(const dump&) = delete;
    dump& operator=(dump&) = delete;

    class UTILITY_API dump_private* __private = nullptr;
public:

    static dump& instance();
    ~dump();

    //! @brief 创建转储文件
    //! @param file
    //! @param error
    static void create(const std::filesystem::path& file);
    static void create(const std::filesystem::path& file, std::error_code& error);

    //! @brief 初始化并向系统注册结构化异常过滤器，用以在崩溃时得到机会创建最小转储文件。
    //! @param callback 当转转储发生后的回调时间，使程序有机会执行某些动作。
    //! @param directory 用于存储转储文件的目录，如果是一个相对路径，则相对于模块目录。
    //! @param filename 转储文件的文件名，可以包含 时间格式化控制字符。
    //! @note 在程序中调用 SetUnhandledExceptionFilter() 会让该方法失效。
    void capture(
        const std::function<void()>& callback = {},
        const std::filesystem::path& directory = "dump", 
        const std::filesystem::path& filename = "%Y%m%d_%H%M%S.dmp");
};

} // win
} // UTILITY_NAMESPACE

#endif // __dump_win_h
