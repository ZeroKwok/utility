// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#include "utility/filesystem.h"
#include "utility/filesystem_file_guard.h"

namespace UTILITY_NAMESPACE {
namespace fs {

file_guard::file_guard()
{}

file_guard::file_guard(file_guard& right)
    : _file(nullptr) {
    std::swap(_file, right._file);
}

file_guard::file_guard(const file& f)
    : _file(f)
{}

file_guard::~file_guard() {
    close();
}

file_guard& file_guard::operator=(file_guard& right) {
    close();
    std::swap(_file, right._file);
    return *this;
}

#if __HAS_CPP11
file_guard::file_guard(file_guard&& right)
    : _file(nullptr) {
    std::swap(_file, right._file);
}

file_guard& file_guard::operator=(file_guard&& right){
    close();
    std::swap(_file, right._file);
}
#endif

void file_guard::close() {
    if (valid())
        fs::close(_file);
    _file = nullptr;
}

bool file_guard::valid() const {
    return _file != nullptr;
}

file_guard::operator bool() const {
    return valid();
}

file_guard::operator file() const {
    return _file;
}

} // fs
} // UTILITY_NAMESPACE
