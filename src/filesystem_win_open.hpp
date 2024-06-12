// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

// #ifndef OS_WIN
// #   define filesystem_win_open_h__
// #endif
#ifndef filesystem_win_open_h__
#define filesystem_win_open_h__

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <Windows.h>

#include "filesystem_p.h"

namespace
{
    DWORD const GENERIC_READ_WRITE = (GENERIC_READ | GENERIC_WRITE);

    struct file_options
    {
        // These are the flags that are used for the osflag of the CRT file
        // object that is created.
        char  crt_flags;

        // These are the flags that are eventually passed to CreateFile to tell
        // the Operating System how to create the file:
        DWORD access;
        DWORD create;
        DWORD share;
        DWORD attributes;
        DWORD flags;
    };
}

static HANDLE __cdecl create_file(
    PCWSTR               const path,
    SECURITY_ATTRIBUTES* const security_attributes,
    file_options         const options
    ) throw()
{
    return CreateFileW(
        path,
        options.access,
        options.share,
        security_attributes,
        options.create,
        options.flags | options.attributes,
        nullptr);
}

static DWORD decode_access_flags(int const oflag) throw()
{
    switch (oflag & (_O_RDONLY | _O_WRONLY | _O_RDWR))
    {
    case _O_RDONLY:
            return GENERIC_READ;

    case _O_WRONLY:

#if 0   // Note: 我们仅提供二进制模式, 因此不需要探测 BOM 
        // If the file is being opened in append mode, we give read access as
        // well because in append (a, not a+) mode, we need to read the BOM to
        // determine the encoding (ANSI, UTF-8, or UTF-16).
        if ((oflag & _O_APPEND) && (oflag & (_O_WTEXT | _O_U16TEXT | _O_U8TEXT)) != 0)
            return GENERIC_READ | GENERIC_WRITE;
#endif
        return GENERIC_WRITE;

    case _O_RDWR:
        return GENERIC_READ | GENERIC_WRITE;
    }

    // This is unreachable, but the compiler can't tell.
    return -1; // "Invalid open flag"
}

static DWORD decode_open_create_flags(int const oflag) throw()
{
    switch (oflag & (_O_CREAT | _O_EXCL | _O_TRUNC))
    {
    case 0:
    case _O_EXCL: // ignore EXCL w/o CREAT
        return OPEN_EXISTING;

    case _O_CREAT:
        return OPEN_ALWAYS;

    case _O_CREAT | _O_EXCL:
    case _O_CREAT | _O_TRUNC | _O_EXCL:
        return CREATE_NEW;

    case _O_TRUNC:
    case _O_TRUNC | _O_EXCL: // ignore EXCL w/o CREAT
        return TRUNCATE_EXISTING;

    case _O_CREAT | _O_TRUNC:
        return CREATE_ALWAYS;
    }

    // This is unreachable, but the compiler can't tell.
    return -1; // "Invalid open flag"
}

#ifndef _SH_DENYRW
#   define _SH_DENYRW      0x10    // deny read/write mode
#   define _SH_DENYWR      0x20    // deny write mode
#   define _SH_DENYRD      0x30    // deny read mode
#   define _SH_DENYNO      0x40    // deny none mode
#   define _SH_SECURE      0x80    // secure mode
#endif

static DWORD decode_sharing_flags(int const shflag, int const access) throw()
{
    switch (shflag)
    {
    case _SH_DENYRW:
        return 0;

    case _SH_DENYWR:
        return FILE_SHARE_READ;

    case _SH_DENYRD:
        return FILE_SHARE_WRITE;

    case _SH_DENYNO:
        return FILE_SHARE_READ | FILE_SHARE_WRITE;

    case _SH_SECURE:
        if (access == GENERIC_READ)
            return FILE_SHARE_READ;
        else
            return 0;
    }

    return -1; // "Invalid sharing flag"
}

// osfile flag values
enum : unsigned char
{
    FOPEN      = 0x01, // file handle open
    FEOFLAG    = 0x02, // end of file has been encountered
    FCRLF      = 0x04, // CR-LF across read buffer (in text mode)
    FPIPE      = 0x08, // file handle refers to a pipe
    FNOINHERIT = 0x10, // file handle opened _O_NOINHERIT
    FAPPEND    = 0x20, // file handle opened O_APPEND
    FDEV       = 0x40, // file handle refers to device
    FTEXT      = 0x80, // file handle is in text mode
};

// The umask value
static int _umaskval = 0666;

static file_options decode_options(int const oflag, int const shflag, int const pmode) throw()
{
    file_options result;
    result.crt_flags  = 0;
    result.access     = decode_access_flags(oflag); 
    result.create     = decode_open_create_flags(oflag);
    result.share      = decode_sharing_flags(shflag, result.access);
    result.attributes = FILE_ATTRIBUTE_NORMAL;
    result.flags      = 0;

    if (oflag & _O_NOINHERIT)
    {
        result.crt_flags |= FNOINHERIT;
    }

    if (oflag & _O_CREAT)
    {
        if (((pmode & ~_umaskval) & _S_IWRITE) == 0)
            result.attributes = FILE_ATTRIBUTE_READONLY;
    }

    if (oflag & _O_TEMPORARY)
    {
        result.flags  |= FILE_FLAG_DELETE_ON_CLOSE;
        result.access |= DELETE;
        result.share  |= FILE_SHARE_DELETE;
    }

    if (oflag & _O_SHORT_LIVED)
    {
        result.attributes |= FILE_ATTRIBUTE_TEMPORARY;
    }

    if (oflag & _O_OBTAIN_DIR)
    {
        result.flags |= FILE_FLAG_BACKUP_SEMANTICS;
    }

    if (oflag & _O_SEQUENTIAL)
    {
        result.flags |= FILE_FLAG_SEQUENTIAL_SCAN;
    }
    else if (oflag & _O_RANDOM)
    {
        result.flags |= FILE_FLAG_RANDOM_ACCESS;
    }

    return result;
}

int _wsopen(
    struct _file*  const file,
    wchar_t const* const path,
    int            const oflag,
    int            const shflag, // default _SH_DENYNO
    int            const pmode,  // file mode
    int            const secure  // default 0
    )
{
    // First, do the initial parse of the options.  The only thing that can fail
    // here is the parsing of the share options, in which case -1 is returned
    // and errno is set.
    file_options options = decode_options(oflag, shflag, pmode);
    if (options.share == static_cast<DWORD>(-1)) {
        return ERROR_INVALID_PARAMETER;  // Invalid argument
    }

    SECURITY_ATTRIBUTES security_attributes;
    security_attributes.nLength = sizeof(security_attributes);
    security_attributes.lpSecurityDescriptor = nullptr;
    security_attributes.bInheritHandle = (oflag & _O_NOINHERIT) == 0;

        // Try to open or create the file:
    HANDLE os_handle = create_file(path, &security_attributes, options);
    if (os_handle == INVALID_HANDLE_VALUE)
    {
#if 0   // Note: 我们仅提供二进制模式, 不需要探测 BOM, 因此不会出现这个问题
        if ((options.access & GENERIC_READ_WRITE) == GENERIC_READ_WRITE && (oflag & _O_WRONLY))
        {
            // The call may have failed because we may be trying to open
            // something for reading that does not allow reading (e.g. a pipe or
            // a device).  So, we try again with just GENERIC_WRITE.  If this
            // succeeds, we will have to assume the default encoding because we
            // will have no way to read the BOM.
            options.access &= ~GENERIC_READ;

            os_handle = create_file(path, &security_attributes, options);
        }
#endif
    }
 
    if (os_handle == INVALID_HANDLE_VALUE) {
        return GetLastError();;
    }

    // Find out what type of file this is (e.g., file, device, pipe, etc.)
    DWORD const file_type = GetFileType(os_handle);

    if (file_type == FILE_TYPE_UNKNOWN)
    {
        DWORD last_error = GetLastError();
        CloseHandle(os_handle);

        // If GetFileType returns FILE_TYPE_UNKNOWN but doesn't fail, the file
        // type really is unknown.  This function is not designed to handle
        // unknown types of files, so we must return an error.
        if (last_error == ERROR_SUCCESS)
            last_error = ERROR_ACCESS_DENIED;
        return last_error;
    }

    if (file_type == FILE_TYPE_CHAR)
    {
        options.crt_flags |= FDEV;
    }
    else if (file_type == FILE_TYPE_PIPE)
    {
        options.crt_flags |= FPIPE;
    }

    // The file is open and valid.  Set the OS handle:
    file->fd = os_handle;

    // Mark the handle as open, and store the flags we gathered so far:
    options.crt_flags |= FOPEN;
    file->flags = options.crt_flags;

    // Set FAPPEND flag if appropriate. Don't do this for devices or pipes:
    if ((options.crt_flags & (FDEV | FPIPE)) == 0 && (oflag & _O_APPEND))
        file->flags |= FAPPEND;

    return true;
}

int _close(struct _file* const file)
{
    if (file->flags & FOPEN)
    {
        // If the underlying handle is INVALID_HANDLE_VALUE, don't try to acutally
        // close it:
        if (file->fd == INVALID_HANDLE_VALUE)
            return 0;

        // https://docs.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
        if (!CloseHandle(file->fd))
            return GetLastError();
    }

    return 0;
}

int _lseeki64(struct _file* const file,  __int64 const offset, int const origin, __int64& offset_output) throw()
{
    LARGE_INTEGER new_pos;
    if (!SetFilePointerEx(file->fd, *reinterpret_cast<LARGE_INTEGER const*>(&offset), &new_pos, origin))
    {
        offset_output = -1;
        return GetLastError();
    }

    offset_output = new_pos.QuadPart;
    return 0;
}

int _read(struct _file* const file, char* const result_buffer, unsigned const result_buffer_size, unsigned& bytes_read) throw()
{
    bytes_read = 0;
    if (result_buffer_size == 0)
        return 0;

    DWORD bytes_read_from_file = 0;
    if (!ReadFile(
            file->fd,
            result_buffer,
            result_buffer_size,
            &bytes_read_from_file,
            nullptr
        ) || bytes_read_from_file > result_buffer_size)
    {
        DWORD const last_error = GetLastError();
        if (last_error == ERROR_ACCESS_DENIED)
        {
            // ERROR_ACCESS_DENIED occurs if the file is open with the wrong
            // read/write mode.  For this error, we should return EBADF, not
            // the EACCES that will be set by __acrt_errno_map_os_error:

            return last_error;

        }
        else if (last_error == ERROR_BROKEN_PIPE)
        {
            // Return 0 if ERROR_BROKEN_PIPE occurs.  It means the handle is
            // a read handle on a pipe for which all write handles have been
            // closed and all data has been read:
            return 0;
        }
        else
        {
            return last_error;
        }
    }
    bytes_read = bytes_read_from_file;

    return 0;
}

int _write(struct _file* const file, void const* const buffer, unsigned const buffer_size, unsigned& bytes_write) throw()
{
    bytes_write = 0;

    // If the buffer is empty, there is nothing to be written:
    if (buffer_size == 0)
        return 0;

    // If the file is opened for appending, seek to the end of the file.  We
    // ignore errors because the underlying file may not allow seeking.
    if (file->flags & FAPPEND) {
        __int64 offset_output;
        _lseeki64(file, 0, FILE_END, offset_output);
    }

    if (!WriteFile(file->fd, buffer, buffer_size, (LPDWORD)&bytes_write, nullptr))
         return GetLastError();

    return 0;
}

#endif // filesystem_win_open_h__