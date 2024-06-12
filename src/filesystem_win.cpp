// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#include "filesystem_p.h"

#ifdef OS_WIN

namespace UTILITY_NAMESPACE {
namespace fs {

#include "filesystem_win_open.hpp"
file open(const path& name, int mode) {
    std::error_code ecode;
    const auto& result = open(name, mode, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Can't open the file");
    return result;
}

file open(const path& name, int oflag, std::error_code& error) noexcept
{
    error.clear();

    if (name.empty()) {
        error = MakeSysError(ERROR_INVALID_PARAMETER);
        return {};
    }

    auto f = new _file{};
    auto c = _wsopen(f, name.c_str(), oflag, _SH_DENYNO, 644, 0);

    if (c != 0)
        error = MakeSysError(c);

    return f;
}

void close(const file& file) noexcept 
{
    if (file == nullptr)
        return;

    _close(file);
    file->flags = 0;
    file->fd = nullptr;
    delete file;
}

size read(const file& file, char* data, int size) 
{
    std::error_code ecode;
    const auto& result = read(file, data, size, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to read file");
    return result;
}

size read(const file& file, char* data, int size, std::error_code& error) noexcept 
{
    error.clear();

    if (file == nullptr) {
        error = MakeSysError(ERROR_INVALID_PARAMETER);
        return {};
    }

    unsigned bytes = 0; 
    auto r = _read(file, data, size, bytes);

    if (r != 0) 
        error = MakeSysError(r);

    return bytes;
}

size write(file& file, const char *data, int size) 
{
    std::error_code ecode;
    const auto& result = write(file, data, size, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to write file");
    return result;
}

size write(file& file, const void *data, int size, std::error_code& error) noexcept 
{
    error.clear();

    if (file == nullptr) {
        error = MakeSysError(ERROR_INVALID_PARAMETER);
        return {};
    }

    unsigned bytes = 0; 
    auto r = _write(file, data, size, bytes);

    if (r != 0) 
        error = MakeSysError(r);

    return bytes;
}

void seek(file& file, size offset, int whence) 
{
    std::error_code ecode;
    seek(file, offset, whence, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to seek the file pointer");
}

void seek(file& file, size offset, int whence, std::error_code& error) noexcept 
{
    error.clear();

    if (file == nullptr) {
        error = MakeSysError(ERROR_INVALID_PARAMETER);
        return;
    }

    __int64 offset_output;
    auto r = _lseeki64(file, offset, whence, offset_output);
    if (r != 0) 
        error = MakeSysError(r);
}

size tell(const file& file) 
{
    std::error_code ecode;
    const auto& result = tell(file, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file pointer");
    return result;
}

size tell(const file& file, std::error_code& error) noexcept 
{
    error.clear();

    if (file == nullptr) {
        error = MakeSysError(ERROR_INVALID_PARAMETER);
        return {};
    }

    __int64 offset_output = 0;
    auto r = _lseeki64(file, 0, FILE_CURRENT, offset_output);
    if (r != 0) 
        error = MakeSysError(r);

    return offset_output;
}

size file_size(const file& file) {
    std::error_code ecode;
    const auto& result = file_size(file, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file size");
    return result;
}

size file_size(const file& file, std::error_code& error) noexcept 
{
    error.clear();

    if (file == nullptr) {
        error = MakeSysError(ERROR_INVALID_PARAMETER);
        return {};
    }

    LARGE_INTEGER large = {};
    if (::GetFileSizeEx(file->fd, &large) != 0)
        return large.QuadPart;
    
    error = MakeSysError(::GetLastError());
    return 0;
}

ftime time(const file& file) 
{
    std::error_code ecode;
    const auto& result = time(file, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file time");
    return result;
}

ftime time(const file& file, std::error_code& error) noexcept 
{
    error.clear();
    
    ftime ft = { -1, -1, -1, -1 };

    if (file == nullptr) {
        error = MakeSysError(ERROR_INVALID_PARAMETER);
        return ft;
    }

    if (::GetFileTime(
        file->fd,
        (LPFILETIME)&ft.create_time,
        (LPFILETIME)&ft.access_time,
        (LPFILETIME)&ft.modify_time) == 0)
    {
        error = MakeSysError(::GetLastError());
        return ft;
    }

    //  FILETIME structure
    //  Contains a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC).
    //  UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME: 0x019DB1DED53E8000
    ft.create_time = (ft.create_time - 0x019DB1DED53E8000) / 10000000;
    ft.access_time = (ft.access_time - 0x019DB1DED53E8000) / 10000000;
    ft.modify_time = (ft.modify_time - 0x019DB1DED53E8000) / 10000000;

    return ft;
}

ftime time(const path& name) 
{
    std::error_code ecode;
    const auto& result = time(name, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file time");
    return result;
}

ftime time(const path& name, std::error_code& error) noexcept 
{
    error.clear();

    if (name.empty()) {
        error = MakeSysError(ERROR_INVALID_PARAMETER);
        return { -1, -1, -1, -1};
    }

    auto f = _file {};
    f.fd = ::CreateFileW(
        name.c_str(),                       // lpFileName
        FILE_READ_ATTRIBUTES,               // dwDesiredAccess
        FILE_SHARE_READ | FILE_SHARE_WRITE, // dwShareMode
        NULL,                               // lpSecurityAttributes
        OPEN_EXISTING,                      // dwCreationDisposition
        FILE_FLAG_BACKUP_SEMANTICS,         // dwFlagsAndAttributes
        NULL);                              // hTemplateFile
    if (f.fd == INVALID_HANDLE_VALUE) {
        error = MakeSysError(::GetLastError());
        return { -1, -1, -1, -1};
    }

    auto ft = time(&f, error);
    if (::CloseHandle(f.fd)) {
        // nothing
    }

    return ft;
}

void set_time(const file& file, const ftime& time) {
    std::error_code ecode;
    set_time(file, time, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to set the file time");
}

void set_time(const file& file, const ftime& time, std::error_code& error) noexcept 
{
    error.clear();
    if (file == nullptr) {
        error = MakeSysError(ERROR_INVALID_PARAMETER);
        return;
    }

    ftime ft = time;
    if (ft.create_time != -1)
        ft.create_time = ft.create_time * 10000000 + 0x019DB1DED53E8000;
    if (ft.access_time != -1)
        ft.access_time = ft.access_time * 10000000 + 0x019DB1DED53E8000;
    if (ft.modify_time != -1)
        ft.modify_time = ft.modify_time * 10000000 + 0x019DB1DED53E8000;

    // If the function succeeds, the return value is nonzero.
    // If the function fails, the return value is zero. To get extended error information, 
    // call GetLastError.
    if (::SetFileTime(
        file->fd,
        (const FILETIME*)&ft.create_time,
        (const FILETIME*)&ft.access_time,
        (const FILETIME*)&ft.modify_time) == 0)
    {
        error = MakeSysError(::GetLastError());
    }
}

void set_time(const path& name, const ftime& time) 
{
    std::error_code ecode;
    set_time(name, time, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to set the file time");
}

void set_time(const path& name, const ftime& time, std::error_code& error) noexcept 
{
    error.clear();
    if (name.empty()) {
        error = MakeSysError(ERROR_INVALID_PARAMETER);
        return;
    }

    auto f = _file {};
    f.fd = ::CreateFileW(
        name.c_str(),                       // lpFileName
        FILE_WRITE_ATTRIBUTES,              // dwDesiredAccess
        FILE_SHARE_READ | FILE_SHARE_WRITE, // dwShareMode
        NULL,                               // lpSecurityAttributes
        OPEN_EXISTING,                      // dwCreationDisposition
        FILE_FLAG_BACKUP_SEMANTICS,         // dwFlagsAndAttributes
        NULL);                              // hTemplateFile
    if (f.fd == INVALID_HANDLE_VALUE) {
        error = MakeSysError(::GetLastError());
        return;
    }

    set_time(&f, time, error);
    if (::CloseHandle(f.fd)) {
        return;
    }
}

bool is_writable(const path& name) 
{
    std::error_code ecode;
    const auto& result = is_writable(name, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Unable to get the file permissions");
    return result;
}

bool is_writable(const path& name, std::error_code& error) noexcept 
{
    error.clear();
    
    if (name.empty()) {
        error = MakeSysError(ERROR_INVALID_PARAMETER);
        return false;
    }

    if (!exists(name, error))
        return path_is_writable(name, error);

    auto fd = ::CreateFileW(
        name.c_str(),                       // lpFileName
        GENERIC_READ | GENERIC_WRITE,       // dwDesiredAccess
        FILE_SHARE_READ | FILE_SHARE_WRITE, // dwShareMode
        NULL,                               // lpSecurityAttributes
        OPEN_EXISTING,                      // dwCreationDisposition
        0,                                  // dwFlagsAndAttributes
        NULL);                              // hTemplateFile

    if (fd == INVALID_HANDLE_VALUE)
    {
        DWORD ecode = ::GetLastError();
        if (ecode != ERROR_ACCESS_DENIED)
            error = MakeSysError(ecode);
        return false;
    }

    return true;
}

} // fs
} // namespace UTILITY_NAMESPACE

#endif // OS_WIN