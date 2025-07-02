// This file is part of the utility project.
// Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#ifndef error_posix_h__
#define error_posix_h__

#include <errno.h>
#include "utility/error.h"

namespace UTILITY_NAMESPACE
{

    std::error_code make_error_from_native(
        const int ecode,
        const std::filesystem::path &filename,
        const error defaultCode)
    {
        // errno 中的值只有在调用的返回值表示错误时, 才有意义(即，大多数系统调用返回 -1; 大多数库函数返回 -1 或 NULL);
        //       一个成功的函数被允许修改 errno。任何系统调用或库函数都不会将 errno 的值设置为 0。
        //
        // Refer: https://man7.org/linux/man-pages/man3/errno.3.html

        switch (ecode)
        {
        case EACCES:
            return make_error(kPermissionError);

        case EINTR:
            return make_error(kInterruptedError);

        case EINVAL:
            return make_error(kInvalidParam);

            // TODO
        }

        return make_error(defaultCode);
    }

} // namespace UTILITY_NAMESPACE

#endif // error_posix_h__