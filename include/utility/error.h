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
    kInvalidParam                        = 0x01, //!< 参数错误
    kRuntimeError                        = 0x02, //!< 运行时错误
    kMemoryError                         = 0x03, //!< 内存错误
    kPermissionError                     = 0x04, //!< 权限错误
    kInterruptedError                    = 0x05, //!< 中断错误, 用户取消/信号中断

    kFilesystemError                     = 0x21, //!< 文件系统错误
    kFilesystemNoSpace                   = 0x22, //!< 文件系统空间不足
    kFilesystemNotWritable               = 0x23, //!< 文件系统不可写, kPermissionError 的具体化
    kFilesystemNotAccessible             = 0x24, //!< 文件系统不可访问, kPermissionError 的具体化
    kFilesystemNotSupportLargeFiles      = 0x25, //!< 文件系统不支持大文件, 一般为超过4GB的文件
    kFilesystemUnavailable               = 0x26, //!< 文件系统不可用
    kFilesystemIOError                   = 0x27, //!< 文件系统IO错误
    kFilesystemNetworkError              = 0x28, //!< 文件系统网络错误
    kFilesystemPathTooLong               = 0x29, //!< 文件系统路径太长

    kFileNotFound                        = 0x41, //!< 文件未找到
    kFileIsMalformed                     = 0x42, //!< 文件损坏
    kFileWasUsedByOtherProcesses         = 0x43, //!< 文件被其他进程使用

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