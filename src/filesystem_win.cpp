// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#include "filesystem_p.h"

#ifdef OS_WIN

namespace UTILITY_NAMESPACE {
namespace fs {

struct _file {
    int mode = 0;
    HANDLE fd = nullptr;
};

file open(const path& name, int mode) {
    std::error_code ecode;
    const auto& result = open(name, mode, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Can't open the file");
    return result;
}

file open(const path& name, int mode, std::error_code& error) noexcept {
    return {};
}

void close(const file& file) noexcept {
    if (file == nullptr)
        return;

    //
    // https://docs.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
    ::CloseHandle(file->fd);

    file->mode = 0;
    file->fd = nullptr;
}

size read(const file& file, char* data, int size) {
    std::error_code ecode;
    const auto& result = read(file, data, size, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to read file");
    return result;
}

size read(const file& file, char* data, int size, std::error_code& error) noexcept {
    return {};
}

size write(file& file, const char *data, int size) {
    std::error_code ecode;
    const auto& result = write(file, data, size, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to write file");
    return result;
}

size write(file& file, const void *data, int size, std::error_code& error) noexcept {
    return {};
}

void seek(file& file, size offset, int whence) {
    std::error_code ecode;
    seek(file, offset, whence, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to seek the file pointer");
}

void seek(file& file, size offset, int whence, std::error_code& error) noexcept {

}

size tell(const file& file) {
    std::error_code ecode;
    const auto& result = tell(file, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file pointer");
    return result;
}

size tell(const file& file, std::error_code& error) noexcept {
    return {};
}

size file_size(const file& file) {
    std::error_code ecode;
    const auto& result = file_size(file, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file size");
    return result;
}

size file_size(const file& file, std::error_code& error) noexcept {
    return {};
}

ftime time(const file& file) {
    std::error_code ecode;
    const auto& result = time(file, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file time");
    return result;
}

ftime time(const file& file, std::error_code& error) noexcept {
    return {};
}

ftime time(const path& name) {
    std::error_code ecode;
    const auto& result = time(name, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file time");
    return result;
}

ftime time(const path& name, std::error_code& error) noexcept {
    return {};
}

void set_time(const file& file, const ftime& time) {
    std::error_code ecode;
    set_time(file, time, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to set the file time");
}

void set_time(const file& file, const ftime& time, std::error_code& error) noexcept {

}

void set_time(const path& name, const ftime& time) {
    std::error_code ecode;
    set_time(name, time, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to set the file time");
}

void set_time(const path& name, const ftime& time, std::error_code& error) noexcept {

}

bool is_writable(const path& name) {
    std::error_code ecode;
    const auto& result = is_writable(name, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file permissions");
    return result;
}

bool is_writable(const path& name, std::error_code& error) noexcept {
    return {};
}


} // fs
} // namespace UTILITY_NAMESPACE

#endif // OS_WIN