/*
*   path_util.ipp
*
*   v0.1 2018-3 by GuoJH
*   v0.2 2019-7 by GuoJH
*   v0.3 2021-1 by GuoJH
*/

#ifdef UTILITY_DISABLE_HEADONLY
#   include "../path_util.h"
#endif

#include <algorithm>
#include <boost/algorithm/string/trim.hpp>
#include <string/string_util.h>
#include <filesystem/path_util.h>
#include <platform/platform_util.h>

#if OS_POSIX
#   include <pwd.h>
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


namespace util {
namespace detail {

#if defined(OS_WIN)
    static const char separator = '/';
    static const char preferred_separator = '\\';
    static const char dot = '.';

#else // Unix-Like & Mac OS

    static const char separator = '/';
    static const char preferred_separator = '/';
    static const char dot = '.';
#endif

    // Windows style
    static const char win_style_preferred_separator = '\\';
    static const char win_style_secondary_separator = '/';

    // Windows style long path
    // http://msdn.microsoft.com/en-us/library/windows/desktop/aa365247%28v=vs.85%29.aspx
    static const char win_style_long_path_prefix[] = "\\\\?\\";

    // Unix-Like style
    static const char unix_style_root = '/';
    static const char unix_style_separator = '/';

    // UNC（Universal Naming Convention）
    // https://en.wikipedia.org/wiki/Path_(computing)#Universal_Naming_Convention
    // https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-dtyp/62e862f4-2a51-452e-8eeb-dc4ff5ee33cc
    static const char unc_prefix = '\\';
    static const char unc_separator = '\\';
    static const char unc_long_path_prefix[] = "\\\\?\\UNC\\";

    // URL (Uniform Resource Locator)
    static const char url_separator = '/';

    template<class _TChar>
    inline size_t find_separator(
        const std::basic_string<_TChar>& path, size_t offest = 0)
    {
         size_t pos1 = path.find(preferred_separator, offest);
         size_t pos2 = path.find(separator, offest);

         return std::min(pos1, pos2);
    }

    template<class _TChar>
    inline size_t rfind_separator(
        const std::basic_string<_TChar>& path, size_t offest = std::basic_string<_TChar>::npos)
    {
        size_t pos1 = path.rfind(preferred_separator, offest);
        size_t pos2 = path.rfind(separator, offest);

        if(pos1 == path.npos)
            return pos2;

        if(pos2 == path.npos)
            return pos1;

        return std::max(pos1, pos2);
    }

    template<class _TChar>
    inline bool is_separator(const _TChar& c)
    {
        return c == separator ||
               c == preferred_separator;
    }

    template<class _TChar>
    inline bool is_win_separator(const _TChar& c)
    {
        return c == win_style_preferred_separator ||
               c == win_style_secondary_separator;
    }

    template<class _TChar>
    inline std::basic_string<_TChar> _path_append(
        const std::basic_string<_TChar>& path,
        const std::basic_string<_TChar>& stem_1)
    {
        typedef typename std::basic_string<_TChar>::value_type char_type;
        if (path.empty())
            return stem_1;

        if (stem_1.empty())
            return path;

        if (detail::is_separator(stem_1[0]))
            return _path_append(path, stem_1.substr(1));

        if (detail::is_separator(path.at(path.length() - 1)))
        {
            if (path.size() >= 2 && detail::is_separator(path.at(path.length() - 2)))
                return _path_append(path.substr(0, path.size() - 1), stem_1);
            else
                return path + stem_1;
        }

#if OS_WIN
        if (path_is_unc(path))
            return path + char_type(detail::unc_separator) + stem_1;

        if (path_is_win_style(path))
            return path + char_type(detail::win_style_preferred_separator) + stem_1;
#endif

        if (path_is_url(path))
            return path + char_type(detail::url_separator) + stem_1;

        if (path_is_unix_style(path))
            return path + char_type(detail::unix_style_separator) + stem_1;

        return path + char_type(detail::preferred_separator) + stem_1;
    }

    template<class _TChar>
    inline std::basic_string<_TChar> _path_find_root(
        const std::basic_string<_TChar>& path)
    {
        if (path.empty())
            return std::basic_string<_TChar>();

        bool is_long = path_is_win_long_path(path);

        if (path_is_unc_style(path))
        {
            size_t pos = 2;
            if (is_long)
                pos = strlen(detail::unc_long_path_prefix) + 1;

            pos = path.find(detail::unc_separator, pos);
            pos = path.find(detail::unc_separator, std::min(pos + 1, path.size()));

            if (pos == std::basic_string<_TChar>::npos)
                return path;

            return path.substr(0, pos); 
        }
        else
        {
            size_t off = 0;
            if (is_long)
                off = strlen(detail::win_style_long_path_prefix) + 1;

            size_t pos = detail::find_separator(path, off);
            return path.substr(0, std::min(pos + 1, path.size()));
        }
    }

    template<class _TChar>
    inline std::basic_string<_TChar> _path_find_parent(
        const std::basic_string<_TChar>& path)
    {
        if (path.empty())
            return std::basic_string<_TChar>();

        size_t pos = path.length() - 1;
        while (pos > 0 && detail::is_separator(path.at(pos)))
            --pos;

        std::basic_string<_TChar> root = _path_find_root(path);
        size_t findPos = detail::rfind_separator(path, pos);

        if (root.length() > findPos)
            return root;

        return path.substr(0, findPos);
    }

    template<class _TChar>
    inline std::basic_string<_TChar> _path_find_filename(
        const std::basic_string<_TChar>& path)
    {
        if (path.empty())
            return std::basic_string<_TChar>();

        size_t pos = path.length() - 1;
        while (pos >= 0 && detail::is_separator(path.at(pos)))
            --pos;

        size_t findPos = detail::rfind_separator(path, pos);
        return path.substr(findPos + 1, pos - findPos);
    }

    template<class _TChar>
    inline std::basic_string<_TChar> _path_find_basename(
        const std::basic_string<_TChar>& path)
    {
        if (path.empty())
            return std::basic_string<_TChar>();

        auto filename = _path_find_filename(path);

        auto dot = filename.rfind(detail::dot);
        if (dot != filename.npos)
            return filename.substr(0, dot);

        return filename;
    }

    template<class _TChar>
    inline std::basic_string<_TChar> _path_find_extension(
        const std::basic_string<_TChar>& path, int flags)
    {
        if (path.empty())
            return std::basic_string<_TChar>();

        size_t pos = 0; 
        if (flags & find_complete)
            pos = path.find(detail::dot);
        else
            pos = path.rfind(detail::dot);

        if (pos != path.npos)
        {
            if (!(flags & find_with_dot))
                ++pos;

            if (flags & find_upper_case)
                return to_upper(path.substr(pos));
            else
                return to_lower(path.substr(pos));
        }

        return std::basic_string<_TChar>();
    }

    template<class _TChar>
    inline std::basic_string<_TChar> _path_filename_trim(
        const std::basic_string<_TChar>& filename,
        const std::basic_string<_TChar>& placeholder)
    {
        if (filename.empty())
            return std::basic_string<_TChar>();

        // POSIX 中对文件名的约束相对与Windows来说宽松的多, 除了不能包括分隔符 / 之外的字符都是合法的, 
        // 但考虑到文件的跨平台存储(短板效应), 这里采用同windows一样的限制.

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
    inline std::basic_string<_TChar> _path_filename_increment(
        const std::basic_string<_TChar>& filename)
    {
        size_t dotPos = filename.rfind('.');
        size_t leftPos = filename.rfind('(');
        size_t rightPos = filename.rfind(')');
        size_t borderPos = detail::rfind_separator(filename);

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

} // detail

fpath path_from_module(intptr_t instance/* = 0*/) 
{
    ferror ferr;
    fpath result = path_from_module(instance, ferr);

    if (ferr)
        throw ferr;
    return result;
}

fpath path_from_module(intptr_t instance, ferror& ferr) noexcept
{
    ferr.clear();

#if OS_WIN
    wchar_t buffer[MAX_PATH + 1] = { 0 };

    //
    // https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamew

    if (::GetModuleFileNameW(reinterpret_cast<HMODULE>(instance), buffer, sizeof buffer) == 0)
    {
        ferr = ferror(::GetLastError(), "Can't get module path");
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
    {
        ferr = ferror(errno, "Can't get module path, readlink() failed");
    }

    return buffer;
#else

    // 不支持 /proc 文件系统的平台上返回程序当前路径

    char buffer[PATH_MAX + 1] = { 0 };

    //
    // https://linux.die.net/man/3/getcwd

    if (::getcwd(buffer, sizeof buffer) == nullptr)
    {
        ferr = ferror(errno, "Can't get module (current work) path, getcwd() failed");
    }

    return buffer;
#endif
}

fpath path_from_module_dir(intptr_t module/* = 0*/)
{
    ferror ferr;
    fpath result = path_from_module_dir(module, ferr);

    if (ferr)
        throw ferr;
    return result;
}

fpath path_from_module_dir(intptr_t module, ferror& ferr) noexcept
{
    return path_find_parent(path_from_module(module, ferr));
}

fpath path_from_module_dir(const fpath& stem)
{
    ferror ferr;
    fpath result = path_find_parent(path_from_module(0, ferr));

    if (ferr)
        throw ferr;

    return path_append(result, stem);
}

fpath path_from_module_dir(const fpath& stem, ferror& ferr) noexcept
{
    return path_append(
        path_find_parent(path_from_module(0, ferr)), stem);
}

fpath path_from_temp()
{
    ferror ferr;
    fpath result = path_from_temp(ferr);

    if (ferr)
        throw ferr;
    return result;
}

fpath path_from_temp(ferror& ferr) noexcept
{
    ferr.clear();

#if OS_WIN
    wchar_t buffer[MAX_PATH] = { 0 };

    if (::GetTempPathW(MAX_PATH, buffer) == 0)
    {
        ferr = ferror(::GetLastError(), "Can't get temp path");
    }
    return buffer;
#else

    std::string buffer = environment_variable("TMPDIR");
    if (!buffer.empty())
        return buffer;

#ifdef P_tmpdir
    // 
    // http://www.gnu.org/software/libc/manual/html_node/Temporary-Files.html#index-P_005ftmpdir-1608

    return P_tmpdir;
#endif

    return "/tmp";
#endif
}

fpath path_from_temp(const fpath& stem)
{
    ferror ferr;
    fpath result = path_from_temp(ferr);

    if (ferr)
        throw ferr;

    return path_append(result, stem);
}

fpath path_from_temp(const fpath& stem, ferror& ferr) noexcept
{
    return path_append(path_from_temp(ferr), stem);
}

fpath path_from_home()
{
    ferror ferr;
    fpath result = path_from_home(ferr);

    if (ferr)
        throw ferr;
    return result;
}

fpath path_from_home(ferror& ferr) noexcept
{
    ferr.clear();

#if OS_WIN
    return path_from_sysdir(CSIDL_PROFILE, ferr);

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
        if (s == 0) // Not found
            ferr = ferror(-1, "Can't read home directory, getpwuid_r() not content found.");
        else
            ferr = ferror(errno = s, "Can't read home directory, getpwuid_r() failed.");

        return fpath();
    }

    return pwd.pw_dir;
#endif
}

fpath path_from_home(const fpath& stem)
{
    ferror ferr;
    fpath result = path_from_home(ferr);

    if (ferr)
        throw ferr;

    return path_append(result, stem);
}

fpath path_from_home(const fpath& stem, ferror& ferr) noexcept
{
    return path_append(path_from_home(ferr), stem);
}

bool path_is_root(const fpath& path) noexcept
{
    if (path_is_unc(path))
    {
        // 1. \\servername\share
        // 2. \\servername\share\
        // 3. \\?\UNC\servername\share

        size_t separator = 0;

        if (path_is_win_long_path(path))
            separator = path.find(detail::unc_separator, strlen(detail::unc_long_path_prefix));
        else
            separator = path.find(detail::unc_separator, 2);

        if (separator != path.npos)
        {
            size_t next_separator = path.find(detail::unc_separator, separator + 1);
            if (next_separator != path.npos)
                return next_separator + 1 == path.size();

            return separator + 1 < path.size();
        }
    }
    else if (path_is_win_style(path) && path.size() >= 2)
    {
        // 1. C:
        // 2. C:\
        // 3. C:/
        // 4. \\?\C:

        auto colon = path.find(':');
        if (colon != path.npos)
        {
            if ((colon + 1) == path.size())
                return true;

            return detail::is_win_separator(path[colon + 1]) && (colon + 2) == path.size();
        }
    }
    else if (path.size() == 1)
    {
        return path[0] == detail::unix_style_root;
    }

    return false;
}

bool path_is_unc(const fpath& path) noexcept
{
    // 1. \\server\share
    // 2. \\?\UNC\server\share

    if (path_is_win_long_path(path))
    {
        if (path.index_of(detail::unc_long_path_prefix) != 0)
            return false;

        auto begin = strlen(detail::unc_long_path_prefix);

        if (path.find(':', begin) != path.npos)
            return false;

        auto separator = path.find(detail::unc_separator, begin);
        if (separator == path.npos)
            return false;

        return separator < path.size();
    }
    else
    {
        // 最短unc路径应该是: \\a\b
        if (path.length() > 5)
        {
            // 以\\打头
            if (path.at(0) == detail::unc_prefix &&
                path.at(1) == detail::unc_prefix)
            {
                auto separator = path.find(detail::unc_separator, 2);
                if (separator == path.npos)
                    return false;

                return (separator + 1) < path.size();
            }
        }
    }

    return false;
}

bool path_is_url(const fpath& path) noexcept
{
    if (path.length() > 4)
    {
        if (path.index_of("://") != fpath::npos)
            return true;
    }

    return false;
}

bool path_is_win_style(const fpath& path) noexcept
{
    auto colon = path.find(':');

    if (colon != path.npos)
    {
        // 不能以冒号开头
        if (colon == 0)
            return false;

        // 冒号前必须是字母(盘符)
        std::locale local("");
        if (!std::isalpha(static_cast<char>(path[colon - 1]), local))
            return false;

        // 盘符只能为1个字母， 且盘符前只能为分割符 （windows驱动器号只有26个）
        if (colon - 1 > 0 && !detail::is_win_separator(path[colon - 2]))
            return false;

        return true;
    }

    return path.find(detail::win_style_preferred_separator) != path.npos;
}

bool path_is_win_long_path(const fpath& path) noexcept
{
    return path.index_of(detail::win_style_long_path_prefix) == 0;
}

bool path_is_unix_style(const fpath& path) noexcept
{
    if (!path.empty() && 
         path.at(0) == detail::unix_style_separator &&
         path.find(detail::win_style_preferred_separator) == path.npos)
    {
        return true;
    }

    if (!path_is_win_style(path))
    {
        if (path.find(detail::unix_style_separator) != fpath::npos)
            return true;
    }

    return false;
}

bool path_is_unc_style(const fpath& path) noexcept
{
    if (path.length() > 3)
    {
        if (path.at(0) == detail::unc_prefix && 
            path.at(1) == detail::unc_prefix)
        {
            if (path_is_win_long_path(path))
                return path.index_of(detail::unc_long_path_prefix) != path.npos;
            else
                return path.find(detail::unix_style_separator) == path.npos;
        }
    }

    return false;
}

bool path_is_remote(const fpath& path) noexcept
{
#if OS_WIN
    if (path.length() < 3)
        return false;

    if (path_is_unc(path))
        return true;

    if (iswalpha(path.at(0)))
    {
        int result = ::GetDriveTypeW(path_find_root(path).c_str());
        if (result == DRIVE_REMOTE)
            return true;
    }
#endif

    return false;
}

bool path_is_writable(const fpath& path)
{
    ferror ferr;
    bool result = path_is_writable(path, ferr);

    if (ferr)
        throw ferr;

    return result;
}

bool path_is_writable(const fpath& path, ferror& ferr) noexcept
{
    ferr.clear();

    fpath dir  = path;
    fpath root = path_find_root(dir);

    if (!file_exist(root, ferr))
        return false;

    while (!file_exist(dir, ferr))
        dir = path_find_parent(dir);

    fpath file = path_append(dir, L".2bcb023e-23f9-42f4-87f7-90d94005accb");
    do
    {
        file_open(file, O_WRONLY | O_CREAT, ferr);
        if (ferr) 
            break;

        file_remove(file, ferr);
        if (ferr) 
            break;

        return true;
    } while (false);

    if (ferr)
    {
        switch (ferr.code())
        {
#if OS_POSIX
        case EACCES:
#else
        case ERROR_ACCESS_DENIED:
#endif
            ferr.clear();
            break;
        }
    }

    return false;
}

fstype path_filesystem(const fpath& path)
{
    ferror ferr;
    fstype result = path_filesystem(path, ferr);

    if (ferr)
        throw ferr;

    return result;
}

fstype path_filesystem(const fpath& path, ferror& ferr) noexcept
{
    ferr.clear();

    fpath root = path_find_root(path);

    if (!file_exist(root, ferr))
        return none;

    if (!detail::is_win_separator(root.back()))
        root.append(1, detail::win_style_preferred_separator);

    std::wstring fsname(10, 0);
    if (!GetVolumeInformationW(
        root.c_str(), nullptr, 0, nullptr, nullptr, nullptr, 
        &fsname[0], fsname.size()))
    {
        ferr = ferror(::GetLastError(), "Can't get volume information.");
        return none;
    }

    // FAT（FAT12\FAT16和FAT32）、NTFS、CDFS、exFAT、
    // RAW、Ext、Btrfs、ZFS、HFS、 HFS+、ReiserFS、JFS、VMFS、XFS、UFS、VXFS、ReFS、WBFS、PFS
    // 
    if (util::start_with(fsname, L"FAT16"))
        return FAT16;
    else if (util::start_with(fsname, L"FAT32"))
        return FAT32;
    else if (util::start_with(fsname, L"exFAT"))
        return exFAT;
    else if (util::start_with(fsname, L"FAT"))
        return FAT;
    else if (util::start_with(fsname, L"NTFS"))
        return NTFS;    
    else if (util::start_with(fsname, L"CDFS"))
        return CDFS;
    else
    {
        ferr = ferror(-1, "retrieve volume information successfully, unprocessed file system type.");
        return none;
    }
}

std::string  path_append(
    const std::string& path, const std::string& stem_1) noexcept
{
    return detail::_path_append(path, stem_1);
}

std::wstring path_append(
    const std::wstring& path, const std::wstring& stem_1) noexcept
{
    return detail::_path_append(path, stem_1);
}

std::string  path_append(
    const std::string& path, 
    const std::string& stem_1, 
    const std::string& stem_2) noexcept
{
    return detail::_path_append(detail::_path_append(path, stem_1), stem_2);
}

std::wstring path_append(
    const std::wstring& path, 
    const std::wstring& stem_1, 
    const std::wstring& stem_2) noexcept
{
    return detail::_path_append(detail::_path_append(path, stem_1), stem_2);
}

std::string  path_find_root(const std::string& path) noexcept
{
    return detail::_path_find_root(path);
}

std::wstring path_find_root(const std::wstring& path) noexcept
{
    return detail::_path_find_root(path);
}

std::string  path_find_parent(const std::string& path) noexcept
{
    return detail::_path_find_parent(path);
}

std::wstring path_find_parent(const std::wstring& path) noexcept
{
    return detail::_path_find_parent(path);
}

std::string  path_find_filename(const std::string& path) noexcept
{
    return detail::_path_find_filename(path);
}

std::wstring path_find_filename(const std::wstring& path) noexcept
{
    return detail::_path_find_filename(path);
}

std::string  path_find_basename(const std::string& path) noexcept
{
    return detail::_path_find_basename(path);
}

std::wstring path_find_basename(const std::wstring& path) noexcept
{
    return detail::_path_find_basename(path);
}

std::string  path_find_extension(
    const std::string& path, int flags /*= find_default*/) noexcept
{
    return detail::_path_find_extension(path, flags);
}

std::wstring path_find_extension(
    const std::wstring& path, int flags /*= find_default*/) noexcept
{
    return detail::_path_find_extension(path, flags);
}

std::string  path_filename_trim(const std::string& filename, const std::string& placeholder /*= ""*/) noexcept
{
    return detail::_path_filename_trim(filename, placeholder);
}

std::wstring path_filename_trim(const std::wstring& filename, const std::wstring& placeholder /*= L""*/) noexcept
{
    return detail::_path_filename_trim(filename, placeholder);
}

std::string  path_filename_increment(const std::string& filename) noexcept
{
    return detail::_path_filename_increment(filename);
}

std::wstring path_filename_increment(const std::wstring& filename) noexcept
{
    return detail::_path_filename_increment(filename);
}

//! 
//! windows 方面的扩展
//! 
#if OS_WIN

fpath path_from_sysdir(int flag/* = 0*/)
{
    ferror ferr;
    fpath result = path_from_sysdir(flag, ferr);

    if (ferr)
        throw ferr;
    return result;
}

fpath path_from_sysdir(int flag, ferror& ferr) noexcept
{
    ferr.clear();

    // reference to shlobj.h line 1204
    int     csidl = (flag == 0 ? CSIDL_DESKTOP : flag);
    wchar_t buffer[MAX_PATH + 1] = { 0 };

    if (!::SHGetSpecialFolderPathW(0, buffer, csidl, true))
    {
        //
        // https://learn.microsoft.com/en-us/windows/win32/api/shlobj_core/nf-shlobj_core-shgetfolderpatha
        //
        if (FAILED(::SHGetFolderPathW(NULL, flag | CSIDL_FLAG_CREATE, NULL, 0, buffer)))
            ferr = ferror(::GetLastError(), "Can't get special folder path");
    }

    return buffer;
}

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA

fpath path_from_sysdir(REFKNOWNFOLDERID rfid)
{
    ferror ferr;
    fpath result = path_from_sysdir(rfid, ferr);

    if (ferr)
        throw ferr;
    return result;
}

fpath path_from_sysdir(REFKNOWNFOLDERID rfid, ferror& ferr) noexcept
{
    ferr.clear();

    //
    // https://docs.microsoft.com/en-us/windows/win32/api/shlobj_core/nf-shlobj_core-shgetknownfolderpath
    //

    PWSTR   path = nullptr;
    HRESULT code = ::SHGetKnownFolderPath(rfid, KF_FLAG_DEFAULT, NULL, &path);
    if (code != S_OK)
    {
        if (code == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            HRESULT hr = SHCreateDirectoryExW(NULL, path, NULL);
            if (hr != S_OK)
                ferr = ferror(code, "Can't get known folder path");
        }
        else
        {
            ferr = ferror(code, "Can't get known folder path");
        }
    }

    fpath result;
    if (path != nullptr)
    {
        result = path;
        ::CoTaskMemFree(path);
    }

    return result;
}

#endif

void path_open_with_explorer(const fpath& path, bool select/* = true*/)
{
    ferror ferr;
    path_open_with_explorer(path, select, ferr);

    if (ferr)
        throw ferr;
}

void path_open_with_explorer(const fpath& path, bool select, ferror& ferr) noexcept
{
    ferr.clear();

    fpath file  = util::replace_copy(path, L"/", L"\\");
    fpath name  = L"\"" + file + L"\"";
    fpath param = L"/select, " + name;

    HINSTANCE hresult = ::ShellExecuteW(NULL, L"open", L"explorer",
        select ? param.data() : name.data(), NULL, SW_SHOWDEFAULT);

    if (hresult <= HINSTANCE(32))
    {
        ferr = ferror(::GetLastError(), "Can't open folder by explorer");
    }
}

#endif // OS_WIN

} // util
