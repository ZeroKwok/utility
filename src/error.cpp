// This file is part of the utility project.
// Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#include "utility/error.h"
#include "utility/string.h"

#if defined(__cpp_lib_format)
#   include <format>
#else
#   include <boost/format.hpp>
#endif

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

std::string error_category::message(int ev) const
{
    if (ev == kSucceed)
        return "Succeed";

#if defined(__cpp_lib_format)
    return std::format("Error: {:#010x}", ev);
#else
    return (boost::format("Error: 0x%08x") % ev).str();
#endif
}

} // namespace UTILITY_NAMESPACE