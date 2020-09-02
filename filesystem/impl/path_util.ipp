#ifdef UTILITY_DISABLE_HEADONLY
#   include "../path_util.hpp"
#endif

#include <algorithm>
#include <filesystem/path_util.hpp>

namespace util {
namespace detail {

#if defined(OS_WIN)
    static const char separator = '/';
    static const char preferred_separator = '\\';
    static const char dot = '.';

#else // Linux & Mac OS

    static const char separator = '/';
    static const char preferred_separator = '/';
    static const char dot = '.';
#endif

    // Windows style
    static const char win_style_separator = '\\';

    // Linux style
    static const char linux_style_root = '/';
    static const char linux_style_separator = '/';

    // UNC£¨Universal Naming Convention£©
    static const char unc_prefix = '\\';
    static const char unc_separator = '\\';

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
    inline std::basic_string<_TChar> _path_append(
        const std::basic_string<_TChar>& path,
        const std::basic_string<_TChar>& stem_1)
    {
        typedef std::basic_string<_TChar>::value_type char_type;
        if (path.empty())
            return stem_1;

        if (stem_1.empty())
            return path;

        if (detail::is_separator(path.at(path.length() - 1)))
            return path + stem_1;

        if (path_is_unc(path))
            return path + char_type(detail::unc_separator) + stem_1;

        if (path_is_url(path))
            return path + char_type(detail::url_separator) + stem_1;

        if (path_is_win_style(path))
            return path + char_type(detail::win_style_separator) + stem_1;

        if (path_is_linux_style(path))
            return path + char_type(detail::linux_style_separator) + stem_1;

        return path + char_type(detail::preferred_separator) + stem_1;
    }

    template<class _TChar>
    inline std::basic_string<_TChar> _path_find_root(
        const std::basic_string<_TChar>& path)
    {
        if (path_is_unc_style(path))
        {
            size_t pos = 2;
            pos = path.find(detail::unc_separator, pos);
            pos = path.find(detail::unc_separator, std::min(pos + 1, path.size()));

            if (pos == std::basic_string<_TChar>::npos)
                return path;

            return path.substr(0, pos);
        }
        else
        {
            size_t pos = detail::find_separator(path, 0);
            return path.substr(0, std::min(pos + 1, path.size()));
        }
    }

    template<class _TChar>
    inline std::basic_string<_TChar> _path_find_parent(
        const std::basic_string<_TChar>& path)
    {
        size_t pos = path.length() - 1;
        while (pos >= 0 && detail::is_separator(path.at(pos)))
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
        size_t pos = path.length() - 1;
        while (pos >= 0 && detail::is_separator(path.at(pos)))
            --pos;

        size_t findPos = detail::rfind_separator(path, pos);
        return path.substr(findPos + 1, pos - findPos);
    }

    template<class _TChar>
    inline std::basic_string<_TChar> _path_find_extension(
        const std::basic_string<_TChar>& path,
        bool hasdot = true,
        bool lower = true)
    {
        size_t pos = path.rfind(detail::dot);

        if (pos != fpath::npos)
        {
            if (!hasdot)
                ++pos;

            if (lower)
                return to_lower(path.substr(pos));
            else
                return to_upper(path.substr(pos));
        }
        return std::basic_string<_TChar>();
    }

    template<class _TChar>
    inline std::basic_string<_TChar> _path_filename_trim(
        const std::basic_string<_TChar>& filename)
    {
        const char   illegal_char[] = "\\/:*?\"<>|";
        const char * illegal_name[] = {
            "con", "prn", "aux", "nul", "com1", "com2",
            "com3", "com4", "com5", "com6", "com7", "com8",
            "com9", "lpt1", "lpt2", "lpt3", "lpt4", "lpt5",
            "lpt6", "lpt7", "lpt8", "lpt9", 0 };

        auto result = filename;
        for (auto it = result.begin(); it != result.end();)
        {
            const char *ch = illegal_char;
            for (; *ch != 0; ++ch)
            {
                if (*it == *ch)
                {
                    it = result.erase(it);
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
                auto ch1_end = result.end();
                auto ch2_begin = illegal_name[i];

                for (; ch1_begin != ch1_end && *ch2_begin != 0; ++ch1_begin, ++ch2_begin)
                {
                    if (*ch1_begin != *ch2_begin)
                        break;
                }

                if (ch1_begin == ch1_end && *ch2_begin == 0)
                {
                    result.insert(0, 1, '_');
                    break;
                }
            }
        }

        return result;
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

        std::basic_string<_TChar> target = filename;

        do
        {
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

            ++leftPos;

            auto subscript = filename.substr(leftPos, rightPos - leftPos);
            if (std::all_of(subscript.begin(), subscript.end(),
                [](unsigned char c) { return std::isdigit(c) || std::isspace(c); }))
            {
                long long number = 1;
                try
                {
                    size_t pos = 0;
                    number = std::stoll(subscript, &pos);
                    number++;
                }
                catch (std::invalid_argument)
                {
                }

                auto string = std::to_string(number);
                return target.replace(leftPos, rightPos - leftPos,
                    std::basic_string<_TChar>(string.begin(), string.end()));
            }
            else
            {
                leftPos = filename.npos;
                rightPos = filename.npos;
                continue;
            }
        } while (1);

        return target; // never get here
    }

#if defined(_MSC_VER)
#   pragma warning (pop)
#endif // defined(_MSC_VER)

} // detail

fpath path_from_module(intptr_t module/* = 0*/) 
{
    ferror ferr;
    fpath result = path_from_module(module, ferr);

    if (ferr)
        throw ferr;
    return result;
}

fpath path_from_module(intptr_t module, ferror& ferr)
{
    ferr.clear();
    wchar_t buffer[MAX_PATH] = { 0 };

    if (::GetModuleFileNameW(reinterpret_cast<HMODULE>(module), buffer, sizeof buffer) == 0)
    {
        ferr = ferror(::GetLastError(), "Can't get module path");
    }
    return buffer;
}

fpath path_from_module_dir(intptr_t module/* = 0*/)
{
    ferror ferr;
    fpath result = path_from_module_dir(module, ferr);

    if (ferr)
        throw ferr;
    return result;
}

fpath path_from_module_dir(intptr_t module, ferror& ferr)
{
    return util::path_find_parent(path_from_module(module, ferr));
}

fpath path_from_temp()
{
    ferror ferr;
    fpath result = path_from_temp(ferr);

    if (ferr)
        throw ferr;
    return result;
}

fpath path_from_temp(ferror& ferr)
{
    ferr.clear();
    wchar_t buffer[MAX_PATH] = { 0 };

    if (::GetTempPathW(MAX_PATH, buffer) == 0)
    {
        ferr = ferror(::GetLastError(), "Can't get module path");
    }
    return buffer;
}

fpath path_from_sysdir(int flag/* = 0*/)
{
    ferror ferr;
    fpath result = path_from_sysdir(flag, ferr);

    if (ferr)
        throw ferr;
    return result;
}

fpath path_from_sysdir(int flag, ferror& ferr)
{
    ferr.clear();

    // reference to shlobj.h line 1204
    int     csidl = (flag == 0 ? CSIDL_DESKTOP : flag);
    wchar_t buffer[MAX_PATH] = { 0 };

    if (!::SHGetSpecialFolderPathW(0, buffer, csidl, true))
        ferr = ferror(::GetLastError(), "Can't get special folder path");

    return buffer;
}

void path_open_with_explorer(const fpath& path, bool select/* = true*/)
{
    ferror ferr;
    path_open_with_explorer(path, select, ferr);

    if (ferr)
        throw ferr;
}

void path_open_with_explorer(const fpath& path, bool select, ferror& ferr)
{
    ferr.clear();
    fpath param = L"/select, " + path;

    HINSTANCE hresult = ::ShellExecuteW(NULL, L"open", L"explorer",
        select ? param.data() : path.data(), NULL, SW_SHOWDEFAULT);

    if (hresult <= HINSTANCE(32))
    {
        ferr = ferror(::GetLastError(), "Can't open folder by explorer");
    }
}

bool path_is_root(const fpath& path)
{
    if (path_is_unc_style(path))
    {
        // R"(\\servername\share)"
        // R"(\\servername\share\)"

        size_t pos = 2;
        if ((pos = path.find(detail::unc_separator, pos)) != fpath::npos)
        {
            size_t next_pos = path.find(detail::unc_separator, std::min(pos + 1, path.size()));
            if (next_pos != fpath::npos)
                return next_pos + 1 == path.size();

            return pos + 1 < path.size();
        }
    }
    else if (path_is_win_style(path) && path.size() >= 3)
    {
        // R"(C:\)"
        // R"(C:/)"
        size_t pos = 0;
        if ((pos = path.find(':', pos)) != fpath::npos)
        {
            return detail::is_separator(path[pos + 1]) && (pos + 2) == path.size();
        }
    }
    else if (path.size() == 1)
    {
        return path[0] == detail::linux_style_root;
    }

    return false;
}

bool path_is_unc(const fpath& path)
{
    // Universal Naming Convention (UNC)
    // https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-dfsc/149a3039-98ce-491a-9268-2f5ddef08192

    if (path.length() > 5)
    {
        if (path.at(0) == detail::unc_prefix && path.at(1) == detail::unc_prefix)
        {
            size_t pos = path.find(detail::unc_separator, 2);
            if (pos != fpath::npos && pos != path.size())
                return true;
        }
    }

    return false;
}

bool path_is_url(const fpath& path)
{
    if (path.length() > 4)
    {
        if (path.find(L"://") != fpath::npos)
            return true;
    }

    return false;
}

bool path_is_win_style(const fpath& path)
{
    return path.find(':') != fpath::npos || 
           path.find(detail::win_style_separator) != fpath::npos;
}

bool path_is_linux_style(const fpath& path)
{
    if (!path.empty() && path.at(0) == detail::linux_style_separator)
        return true;

    if (!path_is_win_style(path))
    {
        if (path.find(detail::linux_style_separator) != fpath::npos)
            return true;
    }

    return false;
}

bool path_is_unc_style(const fpath& path)
{
    if(path.length() > 3)
    {
        if(path.at(0) == detail::unc_prefix && path.at(1) == detail::unc_prefix)
            return path.find(detail::linux_style_separator) == fpath::npos;
    }

    return false;
}

bool path_is_remote(const fpath& path)
{
    if (path.length() < 3)
        return false;

    if (path_is_unc(path))
        return true;

    if (std::iswalpha(path.at(0)))
    {
        int result = ::GetDriveTypeW(path_find_root(path).c_str());
        if (result == DRIVE_REMOTE)
            return true;
    }

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

bool path_is_writable(const fpath& path, ferror& ferr)
{
    ferr.clear();

    fpath dir = path;
    fpath root = path_find_root(dir);

    if (!file_exist(root, ferr))
        return false;

    while (!file_exist(dir, ferr))
        dir = path_find_parent(dir);

    fpath file = path_append(dir, L"2BCB023E-23F9-42F4-87F7-90D94005ACCB");
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

    if (ferr && ferr.code() == ERROR_ACCESS_DENIED)
        ferr.clear();

    return false;
}

std::string  path_append(
    const std::string& path, const std::string& stem_1)
{
    return detail::_path_append(path, stem_1);
}

std::wstring path_append(
    const std::wstring& path, const std::wstring& stem_1)
{
    return detail::_path_append(path, stem_1);
}

std::string  path_append(
    const std::string& path, 
    const std::string& stem_1, 
    const std::string& stem_2)
{
    return detail::_path_append(detail::_path_append(path, stem_1), stem_2);
}

std::wstring path_append(
    const std::wstring& path, 
    const std::wstring& stem_1, 
    const std::wstring& stem_2)
{
    return detail::_path_append(detail::_path_append(path, stem_1), stem_2);
}

std::string  path_find_root(const std::string& path)
{
    return detail::_path_find_root(path);
}

std::wstring path_find_root(const std::wstring& path)
{
    return detail::_path_find_root(path);
}

std::string  path_find_parent(const std::string& path)
{
    return detail::_path_find_parent(path);
}

std::wstring path_find_parent(const std::wstring& path)
{
    return detail::_path_find_parent(path);
}

std::string  path_find_filename(const std::string& path)
{
    return detail::_path_find_filename(path);
}

std::wstring path_find_filename(const std::wstring& path)
{
    return detail::_path_find_filename(path);
}

std::string  path_find_extension(
    const std::string& path, bool hasdot/* = true*/, bool lower/* = true*/)
{
    return detail::_path_find_extension(path, hasdot, lower);
}

std::wstring path_find_extension(
    const std::wstring& path, bool hasdot/* = true*/, bool lower/*  = true*/)
{
    return detail::_path_find_extension(path, hasdot, lower);
}

std::string  path_filename_trim(const std::string& filename)
{
    return detail::_path_filename_trim(filename);
}

std::wstring path_filename_trim(const std::wstring& filename)
{
    return detail::_path_filename_trim(filename);
}

std::string  path_filename_increment(const std::string& filename)
{
    return detail::_path_filename_increment(filename);
}

std::wstring path_filename_increment(const std::wstring& filename)
{
    return detail::_path_filename_increment(filename);
}

} // util
