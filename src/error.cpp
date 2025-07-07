// This file is part of the utility project.
// Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#include "utility/error.h"
#include "utility/string.h"

#if OS_WIN
#   include "error_win.hpp"
#else
#   include "error_unix.hpp"
#endif

namespace UTILITY_NAMESPACE {

error_category &error_category::instance()
{
    static error_category _imp;
    return _imp;
}

} // namespace UTILITY_NAMESPACE