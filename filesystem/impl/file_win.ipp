#ifdef UTILITY_DISABLE_HEADONLY
#   include "../file_win.h"
#endif

#include <list>
#include <cctype>
#include <cwctype>
#include <aclapi.h>
#include <shlobj.h>
#include <windows.h>
#include <shellapi.h>
#include <string/string_util.h>
#include <filesystem/path_util.h>
#include <platform/platform_util.h>

#include <boost/filesystem.hpp>

#pragma comment(lib, "Version.lib")

namespace util {

void ffile::close()
{
    if (vaild())
    {
        //
        // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-flushfilebuffers
        ::FlushFileBuffers(reinterpret_cast<HANDLE>(native_id()));

        //
        // https://docs.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle

        ::CloseHandle(reinterpret_cast<HANDLE>(native_id()));
    }

    _native_id = -1;
    _open_flags = 0;
}

bool file_exist(const fpath& name)
{
    ferror ferr;
    bool result = file_exist(name, ferr);

    if (ferr)
        throw ferr;

    return result;
}

bool file_exist(const fpath& name, ferror& ferr) noexcept
{
    ferr.clear();

    // Symbolic link behavior—If the path points to a symbolic link, the function returns attributes for the symbolic link.
    //
    // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileattributesa
    // 
    // 这里要修改为, 查找符号链接的内容: GetFileAttributesExW(), filesystem.cpp:__std_fs_get_stats()

    if (::GetFileAttributesW(name.c_str()) != INVALID_FILE_ATTRIBUTES)
    {
        return true;
    }
    else
    {
        WIN32_FIND_DATAW file_data;
        HANDLE fd = ::FindFirstFileW(name.c_str(), &file_data);

        if (fd == INVALID_HANDLE_VALUE)
        {
            DWORD ecode = ::GetLastError();
            switch (ecode)
            {
            case ERROR_INVALID_NAME:    // 文件名语法不正确, 比如包含非法字符
            case ERROR_FILE_NOT_FOUND:
            case ERROR_PATH_NOT_FOUND:
                break;

            default:
                ferr = ferror(ecode, "Can't get file attributes");
                break;
            }

            return false;
        }
        ::FindClose(fd);
    }

    return true;
}

void file_remove(const fpath& name)
{
    ferror ferr;
    file_remove(name, ferr);

    if (ferr)
        throw ferr;
}

void file_remove(const fpath& name, ferror& ferr) noexcept
{
    ferr.clear();

    // If the path points to a symbolic link, the symbolic link is deleted, not the target. 
    //
    // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-deletefile
    // 

    if (::DeleteFileW(name.c_str()) == 0)
    {
        switch (DWORD ecode = ::GetLastError())
        {
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
            break;

        default:
            ferr = ferror(ecode, "Can't delete file");
        }
    }
}

void file_copy(const fpath& from, const fpath& to)
{
    ferror ferr;
    file_copy(from, to, ferr);

    if (ferr)
        throw ferr;
}

void file_copy(const fpath& from, const fpath& to, ferror& ferr) noexcept
{
    ferr.clear();

#if 0
    // Symbolic link behavior—If the source file is a symbolic link, 
    // the actual file copied is the target of the symbolic link.
    //
    // If the destination file already existsand is a symbolic link, 
    // the target of the symbolic link is overwritten by the source file.
    // 
    // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-copyfilew
    // 
    if (::CopyFileW(from.c_str(), to.c_str(), false) == 0) // CopyFile2
    {
        ferr = ferror(::GetLastError(), "Can't copy file");
    }
#endif

    // 以上会复制目标会解析符号链接, 这可能会意外的替换其他地方的文件, 可以考虑通过CopyFileEx()代替.
    // 
    // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-copyfileexa
    //
    // 目前暂时通过boost实现

    boost::system::error_code ecode;
    boost::filesystem::copy_file(
        boost::filesystem::path(from),
        boost::filesystem::path(to),
        boost::filesystem::copy_option::overwrite_if_exists, ecode);

    if (ecode)
    {
        ferr = ferror(ecode.value(), "Can't copy file, copy_file() failed.");
    }
}

void file_move(const fpath& from, const fpath& to)
{
    ferror ferr;
    file_move(from, to, ferr);

    if (ferr)
        throw ferr;
}

void file_move(const fpath& from, const fpath& to, ferror& ferr) noexcept
{
    ferr.clear();

    if (!file_exist(from, ferr))
        return;

    if (file_exist(to, ferr))
    {
        ferr = ferror(-1, "Destination file exists");
        return;
    }

    // 1. 将移动同一个目录中的文件或目录(包括其子目录)或跨目录。
    // 2. 当目标在不同的卷上时，移动目录时MoveFile函数将失败。
    //
    // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-movefilew
    // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-movefileexa

    if (::MoveFileW(from.c_str(), to.c_str()) == 0)
    {
        file_copy(from, to, ferr);
        if (ferr)
            return;

        file_remove(from, ferr);
        if (ferr)
        {
            int code = ferr.code();
            file_remove(to, ferr);

            ferr = ferror(code, "Can't move file, remove source file failed.");
        }
    }
}

fsize file_size(const fpath& name)
{
    ferror ferr;
    fsize  result = file_size(name, ferr);

    if (ferr)
        throw ferr;

    return result;
}

fsize file_size(const fpath& name, ferror& ferr) noexcept
{
    ferr.clear();

    //
    // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew

    ffile file(reinterpret_cast<ffile::native_type>(::CreateFileW(
        name.c_str(),                       // lpFileName
        FILE_READ_ATTRIBUTES,               // dwDesiredAccess
        FILE_SHARE_READ | FILE_SHARE_WRITE, // dwShareMode
        NULL,                               // lpSecurityAttributes
        OPEN_EXISTING,                      // dwCreationDisposition
        FILE_FLAG_BACKUP_SEMANTICS,         // dwFlagsAndAttributes
        NULL)));                            // hTemplateFile

    if (!file.vaild())
    {
        ferr = ferror(::GetLastError(), "Can't open file");
        return 0;
    }

    return file_size(file, ferr);
}

ftime file_time(const fpath& name)
{
    ferror ferr;
    ftime  result = file_time(name, ferr);

    if (ferr)
        throw ferr;

    return result;
}

ftime file_time(const fpath& name, ferror& ferr) noexcept
{
    ferr.clear();
    ffile file(reinterpret_cast<ffile::native_type>(::CreateFileW(
        name.c_str(),                       // lpFileName
        FILE_READ_ATTRIBUTES,               // dwDesiredAccess
        FILE_SHARE_READ | FILE_SHARE_WRITE, // dwShareMode
        NULL,                               // lpSecurityAttributes
        OPEN_EXISTING,                      // dwCreationDisposition
        FILE_FLAG_BACKUP_SEMANTICS,         // dwFlagsAndAttributes
        NULL)));                            // hTemplateFile

    if (!file.vaild())
    {
        ferr = ferror(::GetLastError(), "Can't open file");
        ftime ft = { -1, -1, -1, -1 };
        return ft;
    }

    return file_time(file, ferr);
}

void file_set_time(const fpath& name, const ftime& time)
{
    ferror ferr;
    file_set_time(name, time, ferr);

    if (ferr)
        throw ferr;
}

void file_set_time(const fpath& name, const ftime& time, ferror& ferr) noexcept
{
    ferr.clear();
    ffile file(reinterpret_cast<ffile::native_type>(::CreateFileW(
        name.c_str(),                       // lpFileName
        FILE_WRITE_ATTRIBUTES,              // dwDesiredAccess
        FILE_SHARE_READ | FILE_SHARE_WRITE, // dwShareMode
        NULL,                               // lpSecurityAttributes
        OPEN_EXISTING,                      // dwCreationDisposition
        FILE_FLAG_BACKUP_SEMANTICS,         // dwFlagsAndAttributes
        NULL)));                            // hTemplateFile

    if (!file.vaild())
    {
        ferr = ferror(::GetLastError(), "Can't open file");
        return;
    }

    return file_set_time(file, time, ferr);
}

ffile file_open(const fpath& name, int flags)
{
    ferror ferr;
    ffile result = file_open(name, flags, ferr);

    if (ferr)
        throw ferr;

    return result;
}

ffile file_open(const fpath& name, int flags, ferror& ferr) noexcept
{
    ferr.clear();

    int mode_flag = 0;
    int share_flag = 0;
    int creation_flag = 0;
    int attribute_flag = FILE_ATTRIBUTE_NORMAL;
    int append = 0;

    if (flags & _O_WRONLY)   // write-only
        mode_flag = GENERIC_WRITE;

    else if (flags & _O_RDWR)   // read/write
        mode_flag = GENERIC_READ | GENERIC_WRITE;

    else if (flags & _O_RDONLY) // read-only
        mode_flag = GENERIC_READ;

    else if (_O_RDONLY == 0)
        mode_flag = GENERIC_READ;

    else
    {
        ferr = ferror(-1, "Invalid flags");
        return ffile();
    }

    if (flags & _O_APPEND)
        append = 1;

    if (flags & _O_CREAT)
    {
        if (flags & _O_TRUNC)   // creat and trunc 
            creation_flag = CREATE_ALWAYS;
        else
            creation_flag = OPEN_ALWAYS;
    }
    else
    {
        if (flags & _O_TRUNC && util::file_exist(name, ferr)) // file exists and trunc
            creation_flag = CREATE_ALWAYS;
        else
            creation_flag = OPEN_EXISTING;

        if (ferr)
            return ffile();
    }

    if (flags & _O_EXCL)
    {
        share_flag = 0;
        creation_flag = OPEN_EXISTING;
    }
    else
    {
        // 
        // 只读方式打开, 不能仅 FILE_SHARE_READ 否则可能会出现如下错误
        // 0x02000000 ERROR_SHARING_VIOLATION : 另一个程序正在使用此文件，进程无法访问。
        // 参考: ucrt:lowio\open.cpp
        share_flag = FILE_SHARE_READ | FILE_SHARE_WRITE;
    }

    SECURITY_ATTRIBUTES security_attributes;
    security_attributes.nLength = sizeof(security_attributes);
    security_attributes.lpSecurityDescriptor = nullptr;
    security_attributes.bInheritHandle = true;

    // 
    // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew

    ffile::native_type fid = reinterpret_cast<ffile::native_type>(::CreateFileW(
        name.c_str(),         // lpFileName
        mode_flag,            // dwDesiredAccess
        share_flag,           // dwShareMode
        &security_attributes, // lpSecurityAttributes
        creation_flag,        // dwCreationDisposition
        attribute_flag,       // dwFlagsAndAttributes
        nullptr));            // hTemplateFile

    ffile file(fid, flags);

    if (!file.vaild())
    {
        ferr = ferror(::GetLastError(), "Can't open file");
        return ffile();
    }

    if (append != 0)
    {
        file_seek(file, 0, FILE_END, ferr);
    }

    return file;
}

void file_close(ffile& file) noexcept
{
    file.close();
}

void file_read(const ffile& file, void* data_out, int size)
{
    ferror ferr;
    file_read(file, data_out, size, ferr);

    if (ferr)
        throw ferr;
}

void file_read(const ffile& file, void* data_out, int size, ferror& ferr) noexcept
{
    ferr.clear();
    if (!file.vaild())
    {
        ferr = ferror(-1, "Invalid file handle");
        return;
    }

    DWORD read = 0;
    if (!::ReadFile(reinterpret_cast<void*>(file.native_id()), data_out, (DWORD)size, &read, NULL))
    {
        ferr = ferror(::GetLastError(), "File read failed");
        return;
    }

    if (static_cast<int>(read) != size)
    {
        ferr = ferror(-1, "The file was read successfully, but it was too short");
    }
}

void file_write(ffile& file, const void *data, int size)
{
    ferror ferr;
    file_write(file, data, size, ferr);

    if (ferr)
        throw ferr;
}

void file_write(ffile& file, const void *data, int size, ferror& ferr) noexcept
{
    ferr.clear();
    if (!file.vaild())
    {
        ferr = ferror(-1, "Invalid file handle");
        return;
    }

    if (file.flags() & (_O_WRONLY | _O_RDWR ))
    {
        // 
        // 这里的模拟操作是非原子的, 可能存在一定的安全隐患
        // 
        if (file.flags() & _O_APPEND)
            file_seek(file, 0, FILE_END, ferr);
    }

    DWORD written = 0;
    if (!::WriteFile(
        reinterpret_cast<void*>(file.native_id()), 
        data, 
        (DWORD)size, 
        &written, 
        NULL) && static_cast<int>(written) != size)
    {
        ferr = ferror(::GetLastError(), "File write failed");
    }
}

void file_seek(ffile& file, fsize offset, int whence/* = SEEK_SET*/)
{
    ferror ferr;
    file_seek(file, offset, whence, ferr);

    if (ferr)
        throw ferr;
}

void file_seek(ffile& file, fsize offset, int whence, ferror& ferr) noexcept
{
    ferr.clear();
    if (!file.vaild())
    {
        ferr = ferror(-1, "Invalid file handle");
        return;
    }

    LARGE_INTEGER distance = {0};
    distance.QuadPart = offset;
    if (::SetFilePointerEx(reinterpret_cast<void*>(file.native_id()), distance, NULL, whence) == 0)
    {
        ferr = ferror(::GetLastError(), "Can't set file pointer");
    }
}

fsize file_tell(const ffile& file)
{
    ferror ferr;
    int64_t result = file_tell(file, ferr);

    if (ferr)
        throw ferr;

    return result;
}

fsize file_tell(const ffile& file, ferror& ferr) noexcept
{
    ferr.clear();
    if (!file.vaild())
    {
        ferr = ferror(-1, "Invalid file handle");
        return 0;
    }

    LARGE_INTEGER distance = {0};
    if (::SetFilePointerEx(reinterpret_cast<void*>(file.native_id()), distance, &distance, FILE_CURRENT) == 0)
    {
        ferr = ferror(::GetLastError(), "Can't query file pointer");
    }

    return distance.QuadPart;
}

fsize file_size(const ffile& file)
{
    ferror ferr;
    fsize result = file_size(file, ferr);

    if (ferr)
        throw ferr;

    return result;
}

fsize file_size(const ffile& file, ferror& ferr) noexcept
{
    ferr.clear();
    if (!file.vaild())
    {
        ferr = ferror(-1, "Invalid file handle");
        return 0;
    }

    LARGE_INTEGER large;
    if (::GetFileSizeEx(reinterpret_cast<void*>(file.native_id()), &large) != 0)
        return large.QuadPart;
    else
        ferr = ferror(::GetLastError(), "Can't get file size");

    return 0;
}

ftime file_time(const ffile& file)
{
    ferror ferr;
    ftime result = file_time(file, ferr);

    if (ferr)
        throw ferr;

    return result;
}

ftime file_time(const ffile& file, ferror& ferr) noexcept
{
    ferr.clear();
    ftime ft = { -1, -1, -1, -1 };

    if (!file.vaild())
    {
        ferr = ferror(-1, "Invalid file handle");
        return ft;
    }

    if (::GetFileTime(
        reinterpret_cast<void*>(file.native_id()),
        (LPFILETIME)&ft.create_time,
        (LPFILETIME)&ft.access_time,
        (LPFILETIME)&ft.modify_time) == 0)
    {
        ferr = ferror(::GetLastError(), "Can't get file time");
        return ft;
    }

    /*
    *   FILETIME structure
    *   Contains a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC).
    *    UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME: 0x019DB1DED53E8000
    */
    ft.create_time = (ft.create_time - 0x019DB1DED53E8000) / 10000000;
    ft.access_time = (ft.access_time - 0x019DB1DED53E8000) / 10000000;
    ft.modify_time = (ft.modify_time - 0x019DB1DED53E8000) / 10000000;

    return ft;
}

void file_set_time(const ffile& file, const ftime& time)
{
    ferror ferr;
    file_set_time(file, time, ferr);

    if (ferr)
        throw ferr;
}

void file_set_time(const ffile& file, const ftime& time, ferror& ferr) noexcept
{
    ferr.clear();
    if (!file.vaild())
    {
        ferr = ferror(-1, "Invalid file handle");
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
        reinterpret_cast<void*>(file.native_id()),
        (const FILETIME*)&ft.create_time,
        (const FILETIME*)&ft.access_time,
        (const FILETIME*)&ft.modify_time) == 0)
    {
        ferr = ferror(::GetLastError(), "Can't set file time");
    }
}

bool file_is_writable(const fpath& name)
{
    ferror ferr;
    bool result = file_is_writable(name, ferr);

    if (ferr)
        throw ferr;

    return result;
}

bool file_is_writable(const fpath& name, ferror& ferr) noexcept
{
    ferr.clear();
    if (!file_exist(name, ferr))
        return path_is_writable(name, ferr);

    ffile file(reinterpret_cast<ffile::native_type>(::CreateFileW(
        name.c_str(),                       // lpFileName
        GENERIC_READ | GENERIC_WRITE,       // dwDesiredAccess
        FILE_SHARE_READ | FILE_SHARE_WRITE, // dwShareMode
        NULL,                               // lpSecurityAttributes
        OPEN_EXISTING,                      // dwCreationDisposition
        0,                                  // dwFlagsAndAttributes
        NULL)));                            // hTemplateFile

    if (!file.vaild())
    {
        DWORD ecode = ::GetLastError();
        if (ecode != ERROR_ACCESS_DENIED)
        {
            ferr = ferror(ecode, "Can't open file");
        }

        return false;
    }

    return true;
}

void directories_remove(const fpath& path)
{
    ferror ferr;
    directories_remove(path, ferr);

    if (ferr)
        throw ferr;
}

void directories_remove(const fpath& path, ferror& ferr) noexcept
{
    ferr.clear();

    WIN32_FIND_DATAW file_data;
    HANDLE fd = ::FindFirstFileW((path + L"\\*").c_str(), &file_data);

    if (fd == INVALID_HANDLE_VALUE)
    {
        ferr = ferror(::GetLastError(), "Can't remove the directories");
        return;
    }

    while (fd != INVALID_HANDLE_VALUE)
    {
        if (file_data.cFileName[0] != L'.')
        {
            fpath subpath = path + L"\\" + file_data.cFileName;

            //
            //  若当前枚举到的是一个目录, 那么将其递归处理
            //  否则是一个文件, 这里将其删除
            //
            if (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                directories_remove(subpath, ferr);
            }
            else
            {
                file_remove(subpath, ferr);
            }
        }

        if (::FindNextFileW(fd, &file_data) == FALSE)
            break;
    }
    ::FindClose(fd);

    // And then delete the directory
    if (!ferr && ::RemoveDirectoryW(path.c_str()) == 0)
    {
        ferr = ferror(::GetLastError(), "Can't remove the directories");
    }
}

void directories_create(const fpath& path)
{
    ferror ferr;
    directories_create(path, ferr);

    if (ferr)
        throw ferr;
}

void directories_create(const fpath& path, ferror& ferr) noexcept
{
    ferr.clear();

    if (file_exist(path, ferr) || path_is_root(path))
        return;

    fpath pdir = path;

    if (!file_exist(pdir = path_find_parent(pdir), ferr) && pdir != path)
    {
        directories_create(pdir, ferr);
        if (ferr)
            return;
    }

    if (::CreateDirectoryW(path.c_str(), NULL) == 0)
    {
        switch (::GetLastError())
        {
        case ERROR_ALREADY_EXISTS:
            break;

        default:
            ferr = ferror(::GetLastError(), "Can't create directories");
            break;
        }
    }
}

namespace win {
namespace detail {

inline void _named_object_authorization(
    const std::wstring& name, ferror& ferr) noexcept
{
    ferr.clear();

    enum _SID_TYPE 
    {
        BuiltinGuests,
        AnonymousLogon,
        AuthenticatedUsers,
        BuiltinAdministrators,
        SidTypeCount 
    };

    PSID pSids[SidTypeCount] = {0};
    {
        SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;

        // Builtin Guest SID
        AllocateAndInitializeSid(&SIDAuthNT,
            2,
            SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_GUESTS,
            0, 0, 0, 0, 0, 0,
            &(pSids[BuiltinGuests]) );

        // Anonymous Login
        AllocateAndInitializeSid(&SIDAuthNT,
            1,
            SECURITY_ANONYMOUS_LOGON_RID,
            0, 0, 0, 0, 0, 0, 0,
            &(pSids[AnonymousLogon]) );

        // Authenticated Users(大概是所有标准用户)
        AllocateAndInitializeSid(&SIDAuthNT,
            1,
            SECURITY_AUTHENTICATED_USER_RID,
            0, 0, 0, 0, 0, 0, 0,
            &(pSids[AuthenticatedUsers]) );

        // Builtin Administrators
        AllocateAndInitializeSid(&SIDAuthNT,
            2,
            SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS,
            0, 0, 0, 0, 0, 0,
            &(pSids[BuiltinAdministrators]) );
    }


    EXPLICIT_ACCESS ea[SidTypeCount] = {0};
    {
        ACCESS_MODE AccessMode[] = {
            DENY_ACCESS,  // for built-in guest
            DENY_ACCESS,  // for anonymous login
            GRANT_ACCESS, // for Authenticated User
            GRANT_ACCESS  // for Administrator
        }; 

        ACCESS_MASK AccessPermission[] = {
            GENERIC_ALL,
            GENERIC_ALL,
            GENERIC_ALL,
            GENERIC_ALL 
        };

        TRUSTEE_TYPE TrusteeType[] = {
            TRUSTEE_IS_GROUP,
            TRUSTEE_IS_GROUP,
            TRUSTEE_IS_GROUP,
            TRUSTEE_IS_GROUP 
        };

        for(int i = BuiltinGuests; i < SidTypeCount; ++i)
        {
            ea[i].grfAccessMode = AccessMode[i];
            ea[i].grfAccessPermissions = AccessPermission[i];
            ea[i].grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
            ea[i].Trustee.TrusteeType = TrusteeType[i];
            ea[i].Trustee.TrusteeForm = TRUSTEE_IS_SID;
            ea[i].Trustee.ptstrName = LPTSTR(pSids[i]);
        }
    }

    PACL pNewDACL = NULL;
    DWORD dwResult = SetEntriesInAcl(SidTypeCount, ea, NULL, &pNewDACL);
    if (dwResult != ERROR_SUCCESS)
    {
        ferr = ferror(dwResult, "Can't set access control entries into ACL");
    }
    else
    {
        dwResult = SetNamedSecurityInfoW(
            (LPWSTR)name.c_str(),
            SE_FILE_OBJECT,
            DACL_SECURITY_INFORMATION,
            NULL,
            NULL,
            pNewDACL,
            NULL);

        if(dwResult != ERROR_SUCCESS)
            ferr = ferror(dwResult, "Can't set security information into named object");
    }

    if(pNewDACL != NULL)
        LocalFree(pNewDACL);

    for(int i = BuiltinGuests; i < SidTypeCount; ++i)
    {
        if(pSids[i] != NULL )
            LocalFree(pSids[i]);
    }
}

} // detail

bool file_lock(ffile& file, fsize offset, fsize length)
{
    ferror ferr;
    bool result = file_lock(file, offset, length, ferr);

    if (ferr)
        throw ferr;

    return result;
}

bool file_lock(ffile& file, fsize offset, fsize length, ferror& ferr) noexcept
{
    ferr.clear();

    if (!::LockFile(
        reinterpret_cast<void*>(file.native_id()),
        DWORD(offset),
        DWORD(offset >> 32),
        DWORD(length),
        DWORD(length >> 32)))
    {
        switch (::GetLastError())
        {
            //  The process cannot access the file because another process
            //  has locked a portion of the file.
            //
        case ERROR_LOCK_VIOLATION:
            break;

        default:
            ferr = ferror(::GetLastError(), "Can't lock the file");
            break;
        }
        return false;
    }

    return true;
}

bool file_unlock(ffile& file, fsize offset, fsize length)
{
    ferror ferr;
    bool result = file_unlock(file, offset, length, ferr);

    if (ferr)
        throw ferr;

    return result;
}

bool file_unlock(ffile& file, fsize offset, fsize length, ferror& ferr) noexcept
{
    ferr.clear();

    if (!::UnlockFile(
        reinterpret_cast<void*>(file.native_id()),
        int32_t(offset),
        int32_t(offset >> 32),
        int32_t(length),
        int32_t(length >> 32)))
    {
        switch (::GetLastError())
        {
        case ERROR_NOT_LOCKED: // The segment is already unlocked. 
            return true;

        default:
            ferr = ferror(::GetLastError(), "Can't unlock the file");
            break;
        }
        return false;
    }

    return true;
}

bool file_is_occupied(const fpath& name)
{
    ferror ferr;
    bool result = file_is_occupied(name, ferr);

    if (ferr)
        throw ferr;

    return result;
}

bool file_is_occupied(const fpath& name, ferror& ferr) noexcept
{
    ferr.clear();

    if (!file_exist(name, ferr))
    {
        ferr.clear();
        return false;
    }

    ffile file(reinterpret_cast<ffile::native_type>(::CreateFileW(
        name.c_str(),                   // lpFileName
        GENERIC_READ | GENERIC_WRITE,   // dwDesiredAccess
        FILE_SHARE_READ,                // dwShareMode
        NULL,                           // lpSecurityAttributes
        OPEN_EXISTING,                  // dwCreationDisposition
        0,                              // dwFlagsAndAttributes
        NULL)));                        // hTemplateFile

    if (!file.vaild())
    {
        DWORD ecode = ::GetLastError();

        switch (ecode)
        {
            //
            // SEE MSDN
            // The process cannot access the file because it is being used by another process.
            //
        case ERROR_SHARING_VIOLATION:
            return true;

        default:
            ferr = ferror(ecode, "Can't open file");
            break;
        }
    }

    return false;
}

bool file_open_with_shell(const fpath& name)
{
    ferror ferr;
    bool result = file_open_with_shell(name, ferr);

    if (ferr)
        throw ferr;

    return result;
}

bool file_open_with_shell(const fpath& name, ferror& ferr) noexcept
{
    ferr.clear();

    if (::ShellExecuteW(NULL, L"open", name.data(), NULL, NULL, SW_SHOWNORMAL) <= HINSTANCE(32))
    {
        //文件格式不被支持则返回 ERROR_NO_ASSOCIATION
        ferr = ferror(::GetLastError(), "Can't open file");
        return false;
    }

    return true;
}

fversion file_version(const fpath& name)
{
    ferror ferr;
    fversion result = file_version(name, ferr);

    if (ferr)
        throw ferr;

    return result;
}

fversion file_version(const fpath& name, ferror& ferr) noexcept
{
    /*
    *   这里只读取文件版本及一些flags, 欲读取字符串信息参考:
    *   https://docs.microsoft.com/zh-cn/windows/win32/api/winver/nf-winver-verqueryvaluea#return-value
    */
    ferr.clear();

    fversion version = {0};
    do
    {
        int verSize = 0;
        if ((verSize = GetFileVersionInfoSizeW(name.c_str(), NULL)) == 0)
        {
            ferr = ferror(::GetLastError(), "Can't get the version information for the file");
            break;
        }

        std::string buf(verSize, 0);
        if (::GetFileVersionInfoW(name.c_str(), 0, verSize, &buf[0]) == 0)
        {
            ferr = ferror(::GetLastError(), "Can't get the version information for the file");
            break;
        }

        VS_FIXEDFILEINFO * pfInfo = nullptr;
        unsigned int length = sizeof VS_FIXEDFILEINFO;
        if (::VerQueryValueW(buf.data(), L"\\", (LPVOID *)&pfInfo, &length))
        {
            version.file_version.version    = int64_t(pfInfo->dwFileVersionMS)    << 32 | int64_t(pfInfo->dwFileVersionLS);
            version.product_version.version = int64_t(pfInfo->dwProductVersionMS) << 32 | int64_t(pfInfo->dwProductVersionLS);
            version.file_flag_mask          = pfInfo->dwFileFlagsMask;
            version.file_flags              = pfInfo->dwFileFlags;
            version.file_os                 = pfInfo->dwFileOS;
            version.file_type               = pfInfo->dwFileType;
            version.file_sub_type           = pfInfo->dwFileSubtype;
            break;
        }
        else
        {
            ferr = ferror(-1, "The file version information does not exist");
        }
    } 
    while (0);

    return version;
}

void link_create(
    const util::fpath& file_path,
    const util::fpath& link_path,
    const util::fpath& icon_path/* = L""*/,
    const util::fpath& run_cmd/* = L""*/, 
    const short        hot_key/* = 0*/,
    const int          show_flag/* = 0*/)
{
    ferror ferr;
    link_create(file_path, link_path, icon_path,
        run_cmd, hot_key, show_flag, ferr);

    if (ferr)
        throw ferr;
}

void link_create(
    const util::fpath& file_path,
    const util::fpath& link_path,
    const util::fpath& icon_path,
    const util::fpath& run_cmd,
    const short        hot_key,
    const int          show_flag,
    ferror           & ferr) noexcept
{
    BOOL bHasInit = ::CoInitialize(NULL) == S_OK;

    IShellLinkW* pShellLink = NULL;
    IPersistFile* pPersistFile = NULL;
    do
    {
        HRESULT hResult = ::CoCreateInstance(
            CLSID_ShellLink,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IShellLinkW,
            (void**)&pShellLink);
        if (FAILED(hResult))
        {
            ferr = util::ferror(hResult, "Object instances cannot be created");
            break;
        }

        pShellLink->SetPath(file_path.c_str());
        pShellLink->SetWorkingDirectory(util::path_find_parent(file_path).c_str());
        pShellLink->SetArguments(run_cmd.c_str());

        if (hot_key != 0)
            pShellLink->SetHotkey(hot_key);

        if (show_flag != 0)
            pShellLink->SetShowCmd(show_flag);

        if (!icon_path.empty())
            pShellLink->SetIconLocation(icon_path.c_str(), 0);

        hResult = pShellLink->QueryInterface(
            IID_IPersistFile,
            (void**)&pPersistFile);
        if (FAILED(hResult))
        {
            ferr = util::ferror(hResult, "Cannot get an object instance");
            break;
        }

        // 扩展名必须为.lnk
        auto saveFile = link_path;
        if (util::path_find_extension(link_path) != L"lnk")
            saveFile += L".lnk";

        if (FAILED(hResult = pPersistFile->Save(saveFile.c_str(), FALSE)))
        {
            ferr = util::ferror(hResult, "Create the link file failed");
            break;
        }
    } while (0);

    if (bHasInit)
        ::CoUninitialize();

    if (pPersistFile)
        pPersistFile->Release();

    if (pShellLink)
        pShellLink->Release();
}

void directory_authorization(const fpath& path) 
{
    ferror ferr;
    directory_authorization(path, ferr);

    if (ferr)
        throw ferr;
}

void directory_authorization(const fpath& path, ferror& ferr) noexcept
{
    detail::_named_object_authorization(path, ferr);
}

} // win
} // util