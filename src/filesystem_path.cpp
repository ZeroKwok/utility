// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#include "filesystem_p.h"

#if OS_POSIX
#   include <errno.h>
#   include <unistd.h>
#   include <sys/types.h>

#   ifndef PATH_MAX
#       define PATH_MAX 4080
#   endif
#elif OS_WIN
#   include <wctype.h>
#   include <shlobj.h>
#   include <windows.h>
#   include <shellapi.h>
#endif

#include <algorithm>
#include <boost/algorithm/string/trim.hpp>

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

    if (instance != 0)
        sprintf(arg, "/proc/%d/exe", pid_t(instance));

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
    std::string buffer = environment_variable("HOME");
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
        throw MakeFSError(ecode, "Can't get home path");
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

    auto file = dir / L".utility{2bcb023e-23f9-42f4-87f7-90d94005accb}";
    do
    {
        close(open(file, O_WRONLY | O_CREAT, error));
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

template<class _TChar>
inline std::basic_string<_TChar> _filename_trim(
    const std::basic_string<_TChar>& filename,
    const std::basic_string<_TChar>& placeholder)
{
    if (filename.empty())
        return {};

    // POSIX 中对文件名的约束相对与 Windows 来说宽松的多, 除了不能包括分隔符 / 之外的字符都是合法的, 
    // 但考虑到文件的跨平台存储(短板效应), 这里采用同 windows 一样的限制.

    // ASCII 控制字符
    // https://en.cppreference.com/w/cpp/string/byte/iscntrl
    // 
    const char   illegal_char[] = "\\/:*?\"<>|"
                                  "\x1\x2\x3\x4\x5\x6\x7\x8" // control codes (NUL, etc.)
                                  "\t"                       // tab (\t)
                                  "\n\v\f\r"                 // whitespaces (\n, \v, \f, \r)
                                  "\xe\xf\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x10\x1a\x1b\x1c\x1d\x1e\x1f" // control codes(\xE–\x1F)
                                  "\x7f";                    // backspace character (DEL)
    const char * illegal_name[] = {
        "con", "prn", "aux", "nul", "com1", "com2",
        "com3", "com4", "com5", "com6", "com7", "com8",
        "com9", "lpt1", "lpt2", "lpt3", "lpt4", "lpt5",
        "lpt6", "lpt7", "lpt8", "lpt9", ".", "..", 0 };

    // 移除前后的空格
    auto result = boost::algorithm::trim_copy(filename);

    for (auto it = result.begin(); it != result.end();)
    {
        const char *ch = illegal_char;
        for (; *ch != 0; ++ch)
        {
            if (*it == *ch)
            {
                if (!placeholder.empty())
                {
					auto copy = it;
                    result.replace(it, ++copy, placeholder);
                    std::advance(it, placeholder.length());
                }
                else
                {
                    // 预防 "|", "/" 这样的单违法字符的文件名
                    if (result.size() != 1)
                        it = result.erase(it);
                    else
                        *it++ = '_';
                }
                break;
            }
        }

        if (*ch == 0)
            ++it;
    }

    if (result.size() < 5)
    {
        for (int i = 0; illegal_name[i] != 0; ++i)
        {
            auto ch1_begin = result.begin();
            auto ch1_end   = result.end();
            auto ch2_begin = illegal_name[i];

            for (; ch1_begin != ch1_end && *ch2_begin != 0; ++ch1_begin, ++ch2_begin)
            {
                if (*ch1_begin != *ch2_begin)
                    break;
            }

            std::wstring aa;
            if (ch1_begin == ch1_end && *ch2_begin == 0)
            {
                result.insert(0, 1, '(');
                result.append(1, ')');
                break;
            }
        }
    }

    // Windows API 不支持...作为文件名, 故也许需要处理这种情况
    // CreateDirectoryW 
    // GetFileAttributesW
    if (std::all_of(result.begin(), result.end(),
        [](decltype(result[0]) x) { return x == '.'; }))
    {
        result.insert(0, 1, '(');
        result.append(1, ')');
    }

    // Windows API 不支持 文件名后面的 ., 故这里需要移除文件名后的.
    for (int i = result.size() - 1; i > 0; --i)
    {
        if (result[i] != '.')
            break;
        result.pop_back();
    }

    return boost::algorithm::trim_copy(result);
}

#if defined(_MSC_VER)
#   pragma warning (push)
#   pragma warning (disable:4244)
#endif // defined(_MSC_VER)

template<class _TChar>
inline std::basic_string<_TChar> _filename_increment(
    const std::basic_string<_TChar>& filename,
    bool ignore_extension)
{
    size_t dotPos = filename.rfind('.');
    size_t leftPos = filename.rfind('(');
    size_t rightPos = filename.rfind(')');
    //size_t borderPos = _rfind_separator(filename);
    size_t borderPos = filename.npos;

    // 忽略扩展名
    if (ignore_extension)
        dotPos = filename.npos;

    // 预防: ./folder/filename -> .(1)/folder/filename
    if (dotPos != filename.npos && borderPos != filename.npos)
    {
        if (dotPos < borderPos)
            dotPos = filename.npos;
    }

    // 预防: ./folder/filename.(4) -> ./folder/filename.(5)
    do
    {
        if (dotPos != filename.npos)
        {
            if (leftPos != filename.npos && dotPos < leftPos)
            {
                leftPos = filename.rfind('(', leftPos - 1);
                continue;
            }

            if (rightPos != filename.npos && dotPos < rightPos)
            {
                rightPos = filename.rfind(')', rightPos - 1);
                continue;
            }
        }
        break;
    } while (1);

    // 预防: 
    // 1) ./folder/filename(1)-backup.txt -> ./folder/filename(2)-backup.txt
    // 2) ./folder/filename(1)-backup     -> ./folder/filename(2)-backup
    if (leftPos != filename.npos && rightPos != filename.npos && leftPos < rightPos)
    {
        do
        {
            size_t pos = leftPos + 1;

            // 判断'(' 至 ')' 全部都是数字, 或空格
            while (pos < rightPos && (std::isdigit((unsigned char)filename[pos]) || std::isspace((unsigned char)filename[pos])))
                pos++;

            if (pos == rightPos)
            {
                // 允许: 
                // 1) "./folder/filename(1) .txt" -> "./folder/filename(2) .txt"
                // 2) "./folder/filename(1) "     -> "./folder/filename(2) "

                size_t endPos = dotPos != filename.npos ? dotPos : filename.size();

                if (pos + 1 == endPos || pos + 2 == endPos)
                    break;
            }

            // 无效的括号对
            leftPos = filename.npos;
            rightPos = filename.npos;
        } while (0);
    }

    auto target = filename;

    // 没有匹配的括号对
    if (leftPos == filename.npos || rightPos == filename.npos || leftPos > rightPos)
    {
        std::basic_string<_TChar> index(3, 0);
        index[0] = '(';
        index[1] = '1';
        index[2] = ')';

        if (dotPos == filename.npos)
            return target + index;

        return target.insert(dotPos, index);
    }
    else
    {
        // 有匹配的括号对
        long long number = 1;

        if (rightPos > leftPos && rightPos - leftPos > 1)
        {
            size_t pos = 0;
            number = std::stoll(filename.substr(++leftPos, rightPos - leftPos), &pos);

            auto string = std::to_string(++number);
            return target.replace(leftPos, rightPos - leftPos,
                std::basic_string<_TChar>(string.begin(), string.end()));
        }
        else
        {
            auto string = std::to_string(number);
            target.insert(++leftPos, std::basic_string<_TChar>(string.begin(), string.end()));
            return target;
        }
    }
}

#if defined(_MSC_VER)
#   pragma warning (pop)
#endif // defined(_MSC_VER)

path filename_increment(const path& path, bool ignore_extension) noexcept {
    return path.parent_path() / _filename_increment(path.filename().wstring(), ignore_extension);
}

std::string filename_trim(
    const std::string& filename, 
    const std::string& placeholder) noexcept
{
    return _filename_trim(filename, placeholder);
}

std::wstring filename_trim(
    const std::wstring& filename, 
    const std::wstring& placeholder) noexcept
{
    return _filename_trim(filename, placeholder);
}

path path_filename_trim(const path& path, const std::string& placeholder) noexcept {
    return path.parent_path() / _filename_trim(path.filename().wstring(), wstr(placeholder));
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

    auto file = replace_copy(path.wstring(), L"/", L"\\");
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
