// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#ifndef filepath_h__
#define filepath_h__

#include "error.h"
#include <filesystem>

namespace UTILITY_NAMESPACE {

typedef std::filesystem::path path;

UTILITY_API path path_from_utf8(const std::string& path);
UTILITY_API path path_from_module(intptr_t module = 0);

} // UTILITY_NAMESPACE

#endif // filepath_h__
