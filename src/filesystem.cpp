// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#include "filesystem_p.h"

namespace UTILITY_NAMESPACE {
namespace fs {

file open(const path& name, int mode) {
    return {};
}

file open(const path& name, int mode, std::error_code& error) noexcept
{
    return {};
}

void close(const file& file) noexcept {
    
}

} // fs
} // UTILITY_NAMESPACE

