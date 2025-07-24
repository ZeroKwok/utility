// This file is part of the utility project.
// Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#include "filesystem_p.h"

#if OS_POSIX
#   include <errno.h>
#   include <unistd.h>
#   include <stdlib.h>
#   include <sys/types.h>
#   include <pwd.h>

#   ifndef PATH_MAX
#       define PATH_MAX 4080
#   endif
#elif OS_WIN
#   include <wctype.h>
#   include <shlobj.h>
#   include <windows.h>
#   include <shellapi.h>
#endif

#include"string.h"

#include <regex>
#include <algorithm>
#include <unordered_set>
#include <boost/algorithm/string.hpp>

namespace UTILITY_NAMESPACE {
namespace fs {

path path_from_utf8(const std::string& str) {
    return path(wstr_u8(str));
}

#ifdef UTILITY_SUPPORT_QT
path path_from(const QString& str) {
    return path(wstr(str));
}
#endif // UTILITY_SUPPORT_QT

path path_from_module(intptr_t module) {
    std::error_code ecode;
    const auto& result = path_from_module(module, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Can't get module path");
    return result;
}

path path_from_module(intptr_t module, std::error_code& error) noexcept 
{
    error.clear();
#if OS_WIN
    wchar_t buffer[MAX_PATH + 1] = { 0 };

    //
    // https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamew
    if (::GetModuleFileNameW(reinterpret_cast<HMODULE>(module), buffer, sizeof buffer) == 0) {
        error = MakeSysError(::GetLastError());
    }
    return buffer;

#elif OS_LINUX || OS_ANDROID
    char arg[30] = "/proc/self/exe";
    char buffer[PATH_MAX + 1] = { 0 };

    if (module != 0)
        sprintf(arg, "/proc/%d/exe", pid_t(module));

    //
    // https://linux.die.net/man/2/readlink

    if (::readlink(arg, buffer, sizeof buffer) == -1)
        error = MakeSysError(errno);

    return buffer;
#else

    // 不支持 /proc 文件系统的平台上返回程序当前路径
    char buffer[PATH_MAX + 1] = { 0 };

    //
    // https://linux.die.net/man/3/getcwd
    if (::getcwd(buffer, sizeof buffer) == nullptr)
    {
        // "Can't get module (current work) path, getcwd() failed"
        error = MakeSysError(errno);
    }

    return buffer;
#endif
}

path path_from_module_dir(intptr_t module) {
    std::error_code ecode;
    const auto& result = path_from_module_dir(module, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Can't get module path");
    return result;
}

path path_from_module_dir(intptr_t module, std::error_code& error) noexcept {
    return path_from_module(module, error).parent_path();
}

path path_from_module_dir(intptr_t module, const path& stems) {
    std::error_code ecode;
    const auto& result = path_from_module_dir(module, stems, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Can't get module path");
    return result;
}

path path_from_module_dir(intptr_t module, const path& stems, std::error_code& error) noexcept {
    auto result = path_from_module_dir(module, error);
    if (error)
        return {};
    result /= stems;
    return result.lexically_normal();
}

path path_from_temp() {
    std::error_code ecode;
    const auto& result = path_from_temp(ecode);
    if (ecode)
        throw MakeFSError(ecode, "Can't get temp path");
    return result;
}

path path_from_temp(std::error_code& error) noexcept {
    error.clear();
    return temp_directory_path(error);
}

path path_from_temp(const path& stems) {
    std::error_code ecode;
    const auto& result = path_from_temp(stems, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Can't get temp path");
    return result;
}

path path_from_temp(const path& stems, std::error_code& error) noexcept {
    auto result = path_from_temp(error);
    if (error)
        return {};
    result /= stems;
    return result.lexically_normal();
}

path path_from_home() {
    std::error_code ecode;
    const auto& result = path_from_home(ecode);
    if (ecode)
        throw MakeFSError(ecode, "Can't get home path");
    return result;
}

path path_from_home(std::error_code& error) noexcept
{
    error.clear();
#if OS_WIN
    return win::path_from_sysdir(CSIDL_PROFILE, error);

#else
    // 优先采用 HOME 环境变量, 再从/etc/passwd中读取
    std::string buffer = ::getenv("HOME");
    if (!buffer.empty())
        return buffer;

    //
    // https://linux.die.net/man/3/getpwuid_r

    size_t bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1)          /* Value was indeterminate */
        bufsize = 16384;        /* Should be more than enough */

    buffer.resize(bufsize);

    struct passwd pwd = {0};
    struct passwd *result = nullptr;

    // On success return zero, and set *result to pwd. 
    // If no matching password record was found, 
    // these functions return 0 and store NULL in *result. In case of error, 
    // an error number is returned, and NULL is stored in *result.

    int s = getpwuid_r(getuid(), &pwd, &buffer[0], buffer.size(), &result);
    if (result == nullptr)
    {
        if (s == 0) // Not found, "Can't read home directory, getpwuid_r() not content found."
            error = MakeSysError(ENOENT);
        else // "Can't read home directory, getpwuid_r() failed."
            error = MakeSysError(errno = s);

        return {};
    }

    return pwd.pw_dir;
#endif
}

path path_from_home(const path& stems) {
    std::error_code ecode;
    const auto& result = path_from_home(stems, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Can't get home path");
    return result;
}

path path_from_home(const path& stems, std::error_code& error) noexcept {
    auto result = path_from_home(error);
    if (error)
        return {};
    result /= stems;
    return result.lexically_normal();
}

bool path_is_writable(const path& path) {
    std::error_code ecode;
    const auto& result = path_is_writable(path, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Can't get home path", path);
    return result;
}

bool path_is_writable(const path& path, std::error_code& error) noexcept
{
    error.clear();
    if (!exists(path.root_path(), error))
        return false;

    auto dir = path;
    while (!exists(dir, error))
        dir = dir.parent_path();

    if (!is_directory(dir, error))
        return false;

    auto file = dir / L".utility{2bcb023e-23f9-42f4-87f7-90d94005accb}";
    do
    {
        close(open(file, O_WRONLY | O_CREAT, 0664, error));
        if (error) 
            break;
        remove(file, error);
        if (error) 
            break;
        return true;
    }
    while (0);

    if (error)
    {
        switch (error.value())
        {
#if OS_POSIX
        case EACCES:
#else
        case ERROR_ACCESS_DENIED:
#endif
            error.clear();
            break;
        }
    }

    return false;
}

// Windows 文件名非法字符（包括控制字符）
// 控制字符: https://en.cppreference.com/w/cpp/string/byte/iscntrl
template<typename CharT>
inline bool is_invalid_char(CharT c) {
    if (c < 32 || c == 127) // Control characters and DEL
        return true;
    
    constexpr CharT invalid_chars[] = {'<', '>', ':', '"', '/', '\\', '|', '?', '*', 0};
    for (const CharT* p = invalid_chars; *p; ++p) {
        if (c == *p)
            return true;
    }
    return false;
}

// POSIX 中对文件名的约束相对与 Windows 来说宽松的多, 除了不能包括分隔符 / 之外的字符都是合法的, 
// 但考虑到文件的跨平台存储(短板效应), 这里采用同 windows 一样的限制.
template<class _TChar>
inline std::basic_string<_TChar> filename_trim(
    const std::basic_string<_TChar>& filename,
    const std::basic_string<_TChar>& placeholder) 
{
    if (filename.empty())
        return {};

    // 移除前后的空格
    std::basic_string<_TChar> result;
    for (const _TChar& c : boost::algorithm::trim_copy(filename))
    {
        if (is_invalid_char(c))
            result += placeholder;
        else
            result += c;
    }
    
    // 去掉末尾的句点（Windows 禁止）
    while (!result.empty() && result.back() == '.')
        result.pop_back();

    // 至少保留一个字符的文件名
    if (result.empty())
        return std::basic_string<_TChar>(1, _TChar('_'));

    // 文件名小于5则检查是否为保留名称
    if (result.size() < 5)
    {
        // Windows 保留文件名（不区分大小写）
        const char * illegal_name[] = {
            "con", "prn", "aux", "nul",
            "com1", "com2", "com3", "com4", "com5", "com6", "com7", "com8", "com9",
            "lpt1", "lpt2", "lpt3", "lpt4", "lpt5", "lpt6", "lpt7", "lpt8", "lpt9",
            0
        };

        auto target = boost::to_lower_copy(result); 
        for (int i = 0; illegal_name[i] != 0; ++i)
        {
            auto start = target.begin();
            auto end = target.end();
            auto p = illegal_name[i];

            for (; start != end && *p != 0; ++start, ++p)
            {
                if (*start != *p)
                    break;
            }

            if (start == end && *p == 0)
                return _TChar('(') + result + _TChar(')');
        }
    }

    return result;
}

path path_filename_trim(const path& filename, const path& placeholder, bool has_parent) noexcept 
{
    if (has_parent)
    {
        const auto& parent  = filename.parent_path();
        const auto& name    = filename.filename();
        const auto& cleaned = filename_trim(name.wstring(), placeholder.wstring());
        return parent / cleaned;
    }

    return filename_trim(filename.wstring(), placeholder.wstring());
}

path path_filename_increment(const path& filename, bool ignore_extension) noexcept
{
    path parent = filename.parent_path();
    path name = filename.filename();
    
    path base, ext;
    if (!ignore_extension) {
        ext = filename.extension();
        base = filename.stem();
    } else {
        base = name;
    }

    // 提取已有编号 (xxx(n))
    std::wregex pattern(LR"((.*)\((\d+)\)$)");
    std::wsmatch match;
    std::wstring target = base.wstring();

    int number = 1;
    if (std::regex_match(target, match, pattern)) {
        base = match[1].str();
        number = std::stoi(match[2].str()) + 1;
    }

    auto new_name = base.wstring() + L"(" + std::to_wstring(number) + L")" + ext.wstring();
    return parent / new_name;
}

#if OS_WIN
namespace win {

path path_from_sysdir(int flag)
{
    std::error_code ecode;
    const auto& result = path_from_sysdir(flag, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Can't get special folder path");
    return result;
}

path path_from_sysdir(int flag, std::error_code& error) noexcept
{
    error.clear();

    // reference to shlobj.h line 1204
    int     csidl = (flag == 0 ? CSIDL_DESKTOP : flag);
    wchar_t buffer[MAX_PATH + 1] = { 0 };

    if (!::SHGetSpecialFolderPathW(0, buffer, csidl, true))
    {
        // See
        // https://learn.microsoft.com/en-us/windows/win32/api/shlobj_core/nf-shlobj_core-shgetfolderpatha
        if (FAILED(::SHGetFolderPathW(NULL, flag | CSIDL_FLAG_CREATE, NULL, 0, buffer))) {
            error = MakeSysError(::GetLastError());
            return {};
        }
    }

    return buffer;
}

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA

path path_from_sysdir(REFKNOWNFOLDERID rfid)
{
    std::error_code ecode;
    const auto& result = path_from_sysdir(rfid, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Can't get system path");
    return result;
}

path path_from_sysdir(REFKNOWNFOLDERID rfid, std::error_code& error) noexcept
{
    error.clear();

    // See
    // https://docs.microsoft.com/en-us/windows/win32/api/shlobj_core/nf-shlobj_core-shgetknownfolderpath

    PWSTR   folder = nullptr;
    HRESULT result = ::SHGetKnownFolderPath(rfid, KF_FLAG_DEFAULT, NULL, &folder);
    if (result != S_OK)
    {
        if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            HRESULT hr = SHCreateDirectoryExW(NULL, folder, NULL);
            if (hr != S_OK) {
                error = MakeSysError(result); //  "Can't get known folder path"
                return {};
            }
        }
        else
        {
            error = MakeSysError(result);
            return {};
        }
    }

    path output;
    if (folder != nullptr) {
        output = folder;
        ::CoTaskMemFree(folder);
    }

    return output;
}

#endif

void path_open_with_explorer(const path& path, bool select)
{
    std::error_code ecode;
    path_open_with_explorer(path, select, ecode);
    if (ecode)
        throw MakeFSError(ecode, "Can't open folder by explorer");
}

void path_open_with_explorer(
    const path& path, bool select, std::error_code& error) noexcept
{
    error.clear();

    auto file = boost::replace_all_copy(path.wstring(), L"/", L"\\");
    auto name = L"\"" + file + L"\"";
    auto param = L"/select, " + name;

    HINSTANCE hresult = ::ShellExecuteW(NULL, L"open", L"explorer",
        select ? param.data() : name.data(), NULL, SW_SHOWDEFAULT);
    if (hresult <= HINSTANCE(32)) {
        error = MakeSysError(::GetLastError());
    }
}

} // win
#endif // OS_WIN

} // fs
} // UTILITY_NAMESPACE
