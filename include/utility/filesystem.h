// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.
//
// v0.1 2018-03
// v0.2 2019-07
// v0.3 2021-01
// v0.4 2024-06

#ifndef filesystem_h__
#define filesystem_h__

#include "error.h"
#include "filesystem_path.h"

namespace UTILITY_NAMESPACE {

//! 文件系统类型
enum fstype
{
    kFSTypeUnknown   = 0,
    kFSTypeFAT       = 1,
    kFSTypeFAT16     = 2,
    kFSTypeFAT32     = 3,
    kFSTypeexFAT     = 4,
    kFSTypeNTFS      = 5,
    kFSTypeCDFS      = 6,
};

/*!
 *  \brief 获取路径所在的文件系统类型.
 *  
 *  \note  暂不支持Unix-like.
 */
UTILITY_API fstype path_filesystem(const path& path);
UTILITY_API fstype path_filesystem(const path& path, std::error_code& error) noexcept;


} // UTILITY_NAMESPACE

#endif // filesystem_path_h__
