// This file is part of the utility project.
// Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#include "utility/filesystem.h"
#include "filesystem_p.h"

namespace UTILITY_NAMESPACE {
namespace fs {

file::file() noexcept
{
}

file::file(const fptr &f) noexcept
    : _fd(f)
{
}

file::~file() noexcept
{
    std::error_code ec;
    close(ec); // ignore error
}

file::file(file &&right) noexcept
    : _fd(right._fd)
{
    right._fd = nullptr;
}

file &file::operator=(file &&right) noexcept
{
    if (this != &right)
    {
        close();
        _fd = right._fd;
        right._fd = nullptr;
    }
    return *this;
}

void file::close()
{
    std::error_code ec;
    close(ec);
    if (ec)
        throw ec;
}

void file::close(std::error_code &ec) noexcept
{
    ec.clear();
    if (!valid())
        return;

    fs::close(_fd, ec);
    if (ec)
        return;
    _fd = nullptr;
}

bool file::valid() const
{
    return _fd != nullptr;
}

file::operator bool() const
{
    return valid();
}

file::operator fptr() const
{
    return _fd;
}

} // fs
} // UTILITY_NAMESPACE
