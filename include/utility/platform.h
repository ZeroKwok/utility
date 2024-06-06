// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#ifndef platform_h__
#define platform_h__

#include "config.h"

#if OS_WIN
#   include "win/dump.h"
#   include "win/service.h"
#   include "win/registry.h"
#endif

#endif // platform_h__