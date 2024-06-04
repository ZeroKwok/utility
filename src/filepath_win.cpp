// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#include "utility/filepath.h"

#ifdef OS_WIN

namespace UTILITY_NAMESPACE {

path path_from_utf8(const std::string& path)
{
    return {};
}

path path_from_module(intptr_t module)
{
    return {};
}

} // namespace UTILITY_NAMESPACE

#endif // OS_WIN