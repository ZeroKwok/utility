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

#ifndef filesystem_path_h__
#define filesystem_path_h__

#include "error.h"
#include <filesystem>

namespace UTILITY_NAMESPACE {

typedef std::filesystem::path path;

UTILITY_API path path_from_utf8(const std::string& str);
UTILITY_API path path_from_module(intptr_t module = 0);
UTILITY_API path path_from_module(intptr_t module, std::error_code& error);
UTILITY_API path path_from_module_dir(intptr_t module, std::error_code& error);
UTILITY_API path path_from_module_dir(intptr_t module, const util::path& relative, std::error_code& error);

} // UTILITY_NAMESPACE

#endif // filesystem_path_h__
