// This file is part of the utility project.
// Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#include "utility/string.h"
#include "utility/filesystem.h"

namespace UTILITY_NAMESPACE {
namespace fs {

// make an error_code
inline std::error_code MakeSysError(int ecode) noexcept {
    return { ecode, std::system_category() };
}

inline filesystem_error MakeFSError(
    int ecode, const char* what, const path& path1 = {}, const path& path2 = {}) {
    return filesystem_error(what, path1, path2, MakeSysError(ecode));
}

inline filesystem_error MakeFSError(
    std::error_code ecode, const char* what, const path& path1 = {}, const path& path2 = {}) {
    return filesystem_error(what, path1, path2, ecode);
}

#if OS_WIN
struct _file
{
    int flags = 0;
    HANDLE fd = INVALID_HANDLE_VALUE;
};
#endif

} // fs
} // UTILITY_NAMESPACE

