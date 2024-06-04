// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#ifndef error_h__
#define error_h__

#include "config.h"
#include <filesystem>
#include <system_error>

namespace UTILITY_NAMESPACE {

enum error
{
    kSucceed                             = 0x00, //!< 成功
    kUnknownError                        = 0x01, //!< 未知错误
    kInvalidParam                        = 0x02, //!< 参数错误
    kRuntimeError                        = 0x03, //!< 运行时错误
    kOutOfMemory                         = 0x04, //!< 内存不足
    kPermissionDenied                    = 0x05, //!< 权限不足

    kOperationFailed                     = 0x2a, //!< 操作失败
    kOperationInterrupted                = 0x2b, //!< 操作中断, 用户取消

    kFilesystemError                     = 0x51, //!< 文件系统错误
    kFilesystemIOError                   = 0x52, //!< 文件系统IO错误
    kFilesystemUnavailable               = 0x54, //!< 文件系统不可用, 如U盘突然被拔出
    kFilesystemNoSpace                   = 0x55, //!< 本地磁盘空间不足
    kFilesystemNotSupportLargeFiles      = 0x53, //!< 文件系统不支持大文件, 一般为超过4GB的文件
    kFilesystemNetworkError              = 0x56, //!< 文件系统网络错误

    kFileNotFound                        = 0x61, //!< 文件未找到
    kFileNotWritable                     = 0x62, //!< 文件不可写
    kFilePathTooLong                     = 0x63, //!< 文件路径太长
    kFileWasUsedByOtherProcesses         = 0x64, //!< 文件被其他进程使用
    kFileIsMalformed                     = 0x65, //!< 文件损坏

    kNetworkError                        = 0x81, //!< 网络错误
    kNetworkTimeout                      = 0x82, //!< 网络超时
    kNetworkIOError                      = 0x83, //!< 网络IO错误

    kServerError                         = 0xa1, //!< 服务错误
    kServerNotFound                      = 0xa2, //!< 服务不存在
};

class UTILITY_API std::error_category;
class UTILITY_API error_category : public std::error_category {
public:
    static error_category& instance() {
        static error_category _imp;
        return _imp;
    }

    virtual const char* name() const noexcept { return "Error"; }
    virtual std::string message(int ev) const;
};

//! @brief 创建标准错误码对象.
//! @param ecode 由BaseError定义的错误码之一.
//! @return 返回错误码对象.
inline std::error_code make_error(int ecode) {
    return std::error_code(
        static_cast<int>(ecode), error_category::instance());
}

//! @brief 通过本地平台错误码创建错误码对象
//! @param ecode 本地平台的错误码
//! @param filename 如果是文件相关的错误, 则可以携带文件名, 这可以进一步探测错误原因
//! @param defaultCode 默认错误码
//! @return 返回错误码对象
UTILITY_API std::error_code make_error_from_native(
    const int ecode,
    const std::filesystem::path& filename = {},
    const error defaultCode = kRuntimeError);

} // namespace UTILITY_NAMESPACE

#endif // error_h__