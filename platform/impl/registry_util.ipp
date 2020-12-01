#ifdef UTILITY_DISABLE_HEADONLY
#   include "../registry_util.hpp"
#endif

#include <windows.h>
#include <platform/platform_error.hpp>

namespace util{
namespace win {
namespace detail {

class AutoHKEY
{
    AutoHKEY(const AutoHKEY&);
    AutoHKEY& operator=(const AutoHKEY&);

    HKEY _hold;
public:
    AutoHKEY(HKEY handle)
        : _hold(handle)
    {}

    operator HKEY() { return _hold; }
    operator bool() { return _hold != nullptr;  }

    ~AutoHKEY()
    {
        if (_hold != nullptr)
            ::RegCloseKey(_hold);
    }
};

/// ���ַ�������key
inline bool _parse_key(HKEY& key, const tstring& target)
{
    // Predefined keys
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms724836(v=vs.85).aspx

    if (target == L"HKEY_USERS")
        key = HKEY_USERS;
    else if (target == L"HKEY_CURRENT_USER")
        key = HKEY_CURRENT_USER;
    else if (target == L"HKEY_LOCAL_MACHINE")
        key = HKEY_LOCAL_MACHINE;
    else if (target == L"HKEY_CLASSES_ROOT")
        key = HKEY_CLASSES_ROOT;
    else if (target == L"HKEY_CURRENT_CONFIG")
        key = HKEY_CURRENT_CONFIG;
    else if (target == L"HKEY_CURRENT_USER_LOCAL_SETTINGS")
        key = HKEY_CURRENT_USER_LOCAL_SETTINGS;
    else if (target == L"HKEY_PERFORMANCE_DATA")
        key = HKEY_PERFORMANCE_DATA;
    else if (target == L"HKEY_PERFORMANCE_NLSTEXT")
        key = HKEY_PERFORMANCE_NLSTEXT;
    else if (target == L"HKEY_PERFORMANCE_TEXT")
        key = HKEY_PERFORMANCE_TEXT;
    else
        return false;
    
    return true;
}

/// ���ַ�������subkey
inline void _parse_subkey(HKEY& key, tstring& subkey, const tstring& path)
{
    auto pos = std::min(path.find(L"\\"), path.find(L"/"));

    key = HKEY_LOCAL_MACHINE;
    subkey = path;

    if (pos != path.npos)
    {
        if (_parse_key(key, path.substr(0, pos)))
            subkey = path.substr(pos + 1);
    }
}

/// RegOpenKeyExW ��װ
inline HKEY _open_key(
    const tstring& path, REGSAM desiredAccess, platform_error& error)
{
    HKEY result = nullptr;
    {
        HKEY  key;
        tstring subkey;
        _parse_subkey(key, subkey, path);

        LONG retCode = ::RegOpenKeyExW(
            key,
            subkey.c_str(),
            REG_NONE,
            desiredAccess,
            &result);
        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error(retCode, "RegOpenKeyEx failed.");
        }
    }

    return result;
}

inline HKEY _open_or_create_key(
    const tstring& path, REGSAM desiredAccess, platform_error& error)
{
    HKEY result = nullptr;
    {
        HKEY  key;
        tstring subkey;
        _parse_subkey(key, subkey, path);

        LONG retCode = ::RegOpenKeyExW(
            key,
            subkey.c_str(),
            REG_NONE,
            desiredAccess,
            &result);
        if (retCode != ERROR_SUCCESS)
        {
            if (retCode == ERROR_FILE_NOT_FOUND)
            {
                retCode = RegCreateKeyExW(
                    key,
                    subkey.c_str(),
                    0,          // reserved
                    REG_NONE,   // user-defined class type parameter not supported
                    REG_OPTION_NON_VOLATILE,
                    desiredAccess,
                    nullptr,    // no security attributes,
                    &result,
                    nullptr     // no disposition
                );
                if (retCode != ERROR_SUCCESS)
                {
                    error = platform_error(retCode, "RegCreateKeyEx failed.");
                }
            }
            else
            {
                error = platform_error(retCode, "RegOpenKeyEx failed.");
            }
        }
    }

    return result;
}

inline registry_value_types _conver_type(DWORD type)
{
    switch (type)
    {
    case REG_BINARY:                return registr_binary;
    case REG_DWORD:                 return registr_dword;
    case REG_DWORD_BIG_ENDIAN:      return registr_dword;
    case REG_QWORD:                 return registr_qword;
    case REG_SZ:                    return registr_string;
    case REG_LINK:                  return registr_string;
    case REG_EXPAND_SZ:             return registr_expand_string;
    case REG_MULTI_SZ:              return registr_multi_string;
    }

    return registr_binary;
}

inline std::vector<wchar_t> BuildMultiString(
    const std::vector<std::wstring>& data)
{
    // Special case of the empty multi-string
    if (data.empty())
    {
        // Build a vector containing just two NULs
        return std::vector<wchar_t>(2, L'\0');
    }

    // Get the total length in wchar_ts of the multi-string
    size_t totalLen = 0;
    for (const auto& s : data)
    {
        // Add one to current string's length for the terminating NUL
        totalLen += (s.length() + 1);
    }

    // Add one for the last NUL terminator (making the whole structure double-NUL terminated)
    totalLen++;

    // Allocate a buffer to store the multi-string
    std::vector<wchar_t> multiString;

    // Reserve room in the vector to speed up the following insertion loop
    multiString.reserve(totalLen);

    // Copy the single strings into the multi-string
    for (const auto& s : data)
    {
        multiString.insert(multiString.end(), s.begin(), s.end());

        // Don't forget to NUL-terminate the current string
        multiString.emplace_back(L'\0');
    }

    // Add the last NUL-terminator
    multiString.emplace_back(L'\0');

    return multiString;
}

} // detail

bool registry_path_exist(
    const tstring path, int access)
{
    platform_error error;
    auto result = registry_path_exist(path, access, error);

    if (error)
        throw error;

    return result;
}

bool registry_path_exist(
    const tstring path, int access, platform_error& error)
{
    error.clear();

    if (detail::AutoHKEY key = detail::_open_key(path, KEY_READ | access, error))
    {
        return true;
    }

    if (error.code() == ERROR_FILE_NOT_FOUND)
        error.clear();

    return false;
}

bool registry_value_exist(
    const tstring path, const tstring& name, int access)
{
    platform_error error;
    auto result = registry_value_exist(path, name, access, error);

    if (error)
        throw error;

    return result;
}

bool registry_value_exist(
    const tstring path, const tstring& name, int access, platform_error& error)
{
    error.clear();

    if (detail::AutoHKEY key = detail::_open_key(path, KEY_READ | access, error))
    {
        DWORD type;
        DWORD stat;

        if ((stat = ::RegQueryValueExW(key, name.c_str(), 0, &type, 0, 0)) != ERROR_SUCCESS)
        {
            if (stat != ERROR_FILE_NOT_FOUND)
                error = platform_error(stat, "Cannot query values");

            return false;
        }

        return true;
    }

    return false;
}

void registry_tree_remove(const tstring path)
{
    platform_error error;
    registry_tree_remove(path, error);

    if (error)
        throw error;
}

void registry_tree_remove(
    const tstring path, platform_error& error)
{
    error.clear();

    HKEY  key;
    tstring subkey;
    detail::_parse_subkey(key, subkey, path);

    LONG retCode = RegDeleteTreeW(key, subkey.c_str());
    if (retCode != ERROR_SUCCESS)
    {
        error = platform_error{ retCode, "RegDeleteTreeW failed." };
    }
}

void registry_directory_remove(
    const tstring path, int access)
{
    platform_error error;
    registry_directory_remove(path, access, error);

    if (error)
        throw error;
}

void registry_directory_remove(
    const tstring path, int access, platform_error& error)
{
    error.clear();

    HKEY  key;
    tstring subkey;
    detail::_parse_subkey(key, subkey, path);

    LONG retCode = RegDeleteKeyExW(key, subkey.c_str(), KEY_WRITE | access, 0);
    if (retCode != ERROR_SUCCESS)
    {
        error = platform_error{ retCode, "RegDeleteKeyExW failed." };
    }
}

void registry_value_remove(
    const tstring path, const tstring& name, int access)
{
    platform_error error;
    registry_value_remove(path, name, access, error);

    if (error)
        throw error;
}

void registry_value_remove(
    const tstring path, const tstring& name, int access, platform_error& error)
{
    error.clear();

    if (detail::AutoHKEY key = detail::_open_key(path, KEY_WRITE | access, error))
    {
        LONG retCode = RegDeleteValueW(key, name.c_str());
        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error{ retCode, "RegDeleteValue failed." };
        }
    }
}

registry_value_types registry_get_type(
    const tstring path, const tstring& name, int access)
{
    platform_error error;
    auto result = registry_get_type(path, name, access, error);

    if (error)
        throw error;

    return result;
}

registry_value_types registry_get_type(
    const tstring path, const tstring& name, int access, platform_error& error)
{
    error.clear();

    if (detail::AutoHKEY key = detail::_open_key(path, KEY_READ | access, error))
    {
        DWORD type;
        DWORD stat;

        if ((stat = ::RegQueryValueExW(key, name.c_str(), 0, &type, 0, 0)) != ERROR_SUCCESS)
        {
            error = platform_error(stat, "Cannot query values");
            return registr_none;
        }

        return detail::_conver_type(type);
    }

    return registr_none;
}

int32_t registry_get_dword(
    const tstring path, const tstring& name, int access)
{
    platform_error error;
    auto result = registry_get_dword(path, name, access, error);

    if (error)
        throw error;

    return result;
}

int32_t registry_get_dword(
    const tstring path, const tstring& name, int access, platform_error& error)
{
    error.clear();

    if (detail::AutoHKEY key = detail::_open_key(path, KEY_READ | access, error))
    {
        DWORD data = 0;                  // to be read from the registry
        DWORD dataSize = sizeof(data);   // size of data, in bytes

        constexpr DWORD flags = RRF_RT_REG_DWORD;
        LONG retCode = RegGetValueW(
            key,
            nullptr, // no subkey
            name.c_str(),
            flags,
            nullptr, // type not required
            &data,
            &dataSize
        );

        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error( retCode, "Cannot get DWORD value: RegGetValueW failed." );
        }

        return data;
    }

    return 0;
}

int64_t registry_get_qword(
    const tstring path, const tstring& name, int access)
{
    platform_error error;
    auto result = registry_get_qword(path, name, access, error);

    if (error)
        throw error;

    return result;
}

int64_t registry_get_qword(
    const tstring path, const tstring& name, int access, platform_error& error)
{
    error.clear();

    if (detail::AutoHKEY key = detail::_open_key(path, KEY_READ | access, error))
    {
        ULONGLONG data = 0;              // to be read from the registry
        DWORD dataSize = sizeof(data);   // size of data, in bytes

        constexpr DWORD flags = RRF_RT_REG_QWORD;
        LONG retCode = RegGetValueW(
            key,
            nullptr, // no subkey
            name.c_str(),
            flags,
            nullptr, // type not required
            &data,
            &dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error{ retCode, "Cannot get QWORD value: RegGetValueW failed." };
        }

        return data;
    }

    return 0;
}

bytedata registry_get_binary(
    const tstring path, const tstring& name, int access)
{
    platform_error error;
    auto result = registry_get_binary(path, name, access, error);

    if (error)
        throw error;

    return result;
}

bytedata registry_get_binary(
    const tstring path, const tstring& name, int access, platform_error& error)
{
    error.clear();
    bytedata data;

    if (detail::AutoHKEY key = detail::_open_key(path, KEY_READ | access, error))
    {
        // Get the size of the binary data
        DWORD dataSize = 0; // size of data, in bytes
        constexpr DWORD flags = RRF_RT_REG_BINARY;
        LONG retCode = RegGetValueW(
            key,
            nullptr,    // no subkey
            name.c_str(),
            flags,
            nullptr,    // type not required
            nullptr,    // output buffer not needed now
            &dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error{ retCode, "Cannot get size of binary data: RegGetValueW failed." };
            return data;
        }

        // Allocate a buffer of proper size to store the binary data
        data.resize(dataSize);

        // Call RegGetValueW for the second time to read the data content
        retCode = RegGetValueW(
            key,
            nullptr,    // no subkey
            name.c_str(),
            flags,
            nullptr,    // type not required
            &data[0],   // output buffer
            &dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error{ retCode, "Cannot get binary data: RegGetValueW failed." };
        }
    }

    return data;
}

std::wstring registry_get_wstring(
    const tstring path, const tstring& name, int access)
{
    platform_error error;
    auto result = registry_get_wstring(path, name, access, error);

    if (error)
        throw error;

    return result;
}

std::wstring registry_get_wstring(
    const tstring path, const tstring& name, int access, platform_error& error)
{
    error.clear();
    std::wstring result;

    if (detail::AutoHKEY key = detail::_open_key(path, KEY_READ | access, error))
    {
        // Get the size of the result string
        DWORD dataSize = 0; // size of data, in bytes
        DWORD flags  = RRF_RT_REG_SZ;
        LONG retCode = RegGetValueW(
            key,
            nullptr,    // no subkey
            name.c_str(),
            flags,
            nullptr,    // type not required
            nullptr,    // output buffer not needed now
            &dataSize
        );

        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error(retCode, "Cannot get size of string value: RegGetValueW failed.");
            return result;
        }

        // Allocate a string of proper size.
        // Note that dataSize is in bytes and includes the terminating NUL;
        // we have to convert the size from bytes to wchar_ts for wstring::resize.
        result.resize(dataSize / sizeof(wchar_t));
        
        // Call RegGetValueW for the second time to read the string's content
        retCode = RegGetValueW(
            key,
            nullptr,    // no subkey
            name.c_str(),
            flags,
            nullptr,    // type not required
            &result[0], // output buffer
            &dataSize
        );

        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error(retCode, "Cannot get string value: RegGetValueW failed.");
            return result;
        }

        // Remove the NUL terminator scribbled by RegGetValueW from the wstring
        result.resize((dataSize / sizeof(wchar_t)) - 1);
    }

    return result;
}

std::wstring registry_get_expand_wstring(
    const tstring path, const tstring& name, int access)
{
    platform_error error;
    auto result = registry_get_expand_wstring(path, name, access, error);

    if (error)
        throw error;

    return result;
}

std::wstring registry_get_expand_wstring(
    const tstring path, const tstring& name, int access, platform_error& error)
{
    error.clear();
    std::wstring result;

    if (detail::AutoHKEY key = detail::_open_key(path, KEY_READ | access, error))
    {
        // Adjust the flag for RegGetValue considering the expand string option specified by the caller
        DWORD flags = RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND;

        // Get the size of the result string
        DWORD dataSize = 0; // size of data, in bytes
        LONG retCode = RegGetValueW(
            key,
            nullptr,    // no subkey
            name.c_str(),
            flags,
            nullptr,    // type not required
            nullptr,    // output buffer not needed now
            &dataSize
        );

        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error(retCode, "Cannot get size of string value: RegGetValueW failed.");
            return result;
        }

        // Allocate a string of proper size.
        // Note that dataSize is in bytes and includes the terminating NUL;
        // we have to convert the size from bytes to wchar_ts for wstring::resize.
        result.resize(dataSize / sizeof(wchar_t));

        // Call RegGetValueW for the second time to read the string's content
        retCode = RegGetValueW(
            key,
            nullptr,    // no subkey
            name.c_str(),
            flags,
            nullptr,    // type not required
            &result[0], // output buffer
            &dataSize
        );

        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error(retCode, "Cannot get string value: RegGetValueW failed.");
            return result;
        }

        // Remove the NUL terminator scribbled by RegGetValueW from the wstring
        result.resize((dataSize / sizeof(wchar_t)) - 1);
    }

    return result;
}

std::vector<std::wstring> registry_get_multi_wstring(
    const tstring path, const tstring& name, int access)
{
    platform_error error;
    auto result = registry_get_multi_wstring(path, name, access, error);

    if (error)
        throw error;

    return result;
}

std::vector<std::wstring> registry_get_multi_wstring(
    const tstring path, const tstring& name, int access, platform_error& error)
{
    error.clear();
    std::vector<std::wstring> result;

    if (detail::AutoHKEY key = detail::_open_key(path, KEY_READ | access, error))
    {
        // Request the size of the multi-string, in bytes
        DWORD dataSize = 0;
        constexpr DWORD flags = RRF_RT_REG_MULTI_SZ;
        LONG retCode = RegGetValueW(
            key,
            nullptr,    // no subkey
            name.c_str(),
            flags,
            nullptr,    // type not required
            nullptr,    // output buffer not needed now
            &dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error{ retCode, "Cannot get size of multi-string value: RegGetValueW failed." };
            return result;
        }

        // Allocate room for the result multi-string.
        // Note that dataSize is in bytes, but our vector<wchar_t>::resize method requires size
        // to be expressed in wchar_ts.
        std::vector<wchar_t> data(dataSize / sizeof(wchar_t), L' ');

        // Read the multi-string from the registry into the vector object
        retCode = RegGetValueW(
            key,
            nullptr,    // no subkey
            name.c_str(),
            flags,
            nullptr,    // no type required
            &data[0],   // output buffer
            &dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error{ retCode, "Cannot get multi-string value: RegGetValueW failed." };
            return result;
        }

        // Resize vector to the actual size returned by GetRegValue.
        // Note that the vector is a vector of wchar_ts, instead the size returned by GetRegValue
        // is in bytes, so we have to scale from bytes to wchar_t count.
        data.resize(dataSize / sizeof(wchar_t));

        // Parse the double-NUL-terminated string into a vector<wstring>,
        // which will be returned to the caller
        
        const wchar_t* currStringPtr = &data[0];
        while (*currStringPtr != L'\0')
        {
            // Current string is NUL-terminated, so get its length calling wcslen
            const size_t currStringLength = wcslen(currStringPtr);

            // Add current string to the result vector
            result.emplace_back(currStringPtr, currStringLength);

            // Move to the next string
            currStringPtr += currStringLength + 1;
        }
    }

    return result;
}

void registry_set_dword(
    const tstring path, const tstring& name, int32_t value, int access)
{
    platform_error error;
    registry_set_dword(path, name, value, access, error);

    if (error)
        throw error;
}

void registry_set_dword(
    const tstring path, const tstring& name, int32_t value, int access, platform_error& error)
{
    error.clear();

    if (detail::AutoHKEY key = detail::_open_or_create_key(path, KEY_WRITE | access, error))
    {
        LONG retCode = RegSetValueExW(
            key,
            name.c_str(),
            0, // reserved
            REG_DWORD,
            reinterpret_cast<const BYTE*>(&value),
            sizeof(value)
        );
        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error{ retCode, "Cannot write DWORD value: RegSetValueExW failed." };
        }
    }
}

void registry_set_qword(
    const tstring path, const tstring& name, int64_t value, int access)
{
    platform_error error;
    registry_set_qword(path, name, value, access, error);

    if (error)
        throw error;
}

void registry_set_qword(
    const tstring path, const tstring& name, int64_t value, int access, platform_error& error)
{
    error.clear();

    if (detail::AutoHKEY key = detail::_open_or_create_key(path, KEY_WRITE | access, error))
    {
        LONG retCode = RegSetValueExW(
            key,
            name.c_str(),
            0, // reserved
            REG_QWORD,
            reinterpret_cast<const BYTE*>(&value),
            sizeof(value)
        );
        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error{ retCode, "Cannot write QWORD value: RegSetValueExW failed." };
        }
    }
}

void registry_set_binary(
    const tstring path, const tstring& name, const bytedata& value, int access)
{
    platform_error error;
    registry_set_binary(path, name, value, access, error);

    if (error)
        throw error;
}

void registry_set_binary(
    const tstring path, const tstring& name, const bytedata& value, int access, platform_error& error)
{
    error.clear();

    if (detail::AutoHKEY key = detail::_open_or_create_key(path, KEY_WRITE | access, error))
    {
        // Total data size, in bytes
        const DWORD dataSize = static_cast<DWORD>(value.size());

        LONG retCode = RegSetValueExW(
            key,
            name.c_str(),
            0, // reserved
            REG_BINARY,
            (const BYTE*)&value[0],
            dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error{ retCode, "Cannot write binary data value: RegSetValueExW failed." };
        }
    }
}

void registry_set_wstring(
    const tstring path, const tstring& name, const std::wstring& value, int access)
{
    platform_error error;
    registry_set_wstring(path, name, value, access, error);

    if (error)
        throw error;
}

void registry_set_wstring(
    const tstring path, const tstring& name, const std::wstring& value, int access, platform_error& error)
{
    error.clear();

    if (detail::AutoHKEY key = detail::_open_or_create_key(path, KEY_WRITE | access, error))
    {
        // String size including the terminating NUL, in bytes
        const DWORD dataSize = static_cast<DWORD>((value.length() + 1) * sizeof(wchar_t));

        LONG retCode = RegSetValueExW(
            key,
            name.c_str(),
            0, // reserved
            REG_SZ,
            reinterpret_cast<const BYTE*>(value.c_str()),
            dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error{ retCode, "Cannot write string value: RegSetValueExW failed." };
        }
    }
}

void registry_set_multi_wstring(
    const tstring path, const tstring& name, const std::vector<std::wstring>& value, int access)
{
    platform_error error;
    registry_set_multi_wstring(path, name, value, access, error);

    if (error)
        throw error;
}

void registry_set_multi_wstring(
    const tstring path, const tstring& name, const std::vector<std::wstring>& value, int access, platform_error& error)
{
    error.clear();

    if (detail::AutoHKEY key = detail::_open_or_create_key(path, KEY_WRITE | access, error))
    {
        // First, we have to build a double-NUL-terminated multi-string from the input data
        const std::vector<wchar_t> multiString = detail::BuildMultiString(value);

        // Total size, in bytes, of the whole multi-string structure
        const DWORD dataSize = static_cast<DWORD>(multiString.size() * sizeof(wchar_t));

        LONG retCode = RegSetValueExW(
            key,
            name.c_str(),
            0, // reserved
            REG_MULTI_SZ,
            reinterpret_cast<const BYTE*>(&multiString[0]),
            dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error{ retCode, "Cannot write multi-string value: RegSetValueExW failed." };
        }
    }
}

std::vector<std::wstring> registry_get_wkeys(
    const tstring path, int access)
{
    platform_error error;
    auto result = registry_get_wkeys(path, access, error);

    if (error)
        throw error;

    return result;
}

std::vector<std::wstring> registry_get_wkeys(
    const tstring path, int access, platform_error& error)
{
    error.clear();

    // The result subkey names will be stored here
    std::vector<std::wstring> subkeyNames;

    if (detail::AutoHKEY key = detail::_open_key(path, KEY_READ | access, error))
    {
        // Get some useful enumeration info, like the total number of subkeys
        // and the maximum length of the subkey names
        DWORD subKeyCount = 0;
        DWORD maxSubKeyNameLen = 0;
        LONG retCode = RegQueryInfoKey(
            key,
            nullptr,    // no user-defined class
            nullptr,    // no user-defined class size
            nullptr,    // reserved
            &subKeyCount,
            &maxSubKeyNameLen,
            nullptr,    // no subkey class length
            nullptr,    // no value count
            nullptr,    // no value name max length
            nullptr,    // no max value length
            nullptr,    // no security descriptor
            nullptr     // no last write time
        );
        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error{
                retCode,
                "RegQueryInfoKey failed while preparing for subkey enumeration."
            };
        }

        // NOTE: According to the MSDN documentation, the size returned for subkey name max length
        // does *not* include the terminating NUL, so let's add +1 to take it into account
        // when I allocate the buffer for reading subkey names.
        maxSubKeyNameLen++;

        // Preallocate a buffer for the subkey names
        auto nameBuffer = std::make_unique<wchar_t[]>(maxSubKeyNameLen);

        // Reserve room in the vector to speed up the following insertion loop
        subkeyNames.reserve(subKeyCount);

        // Enumerate all the subkeys
        for (DWORD index = 0; index < subKeyCount; index++)
        {
            // Get the name of the current subkey
            DWORD subKeyNameLen = maxSubKeyNameLen;
            retCode = RegEnumKeyExW(
                key,
                index,
                nameBuffer.get(),
                &subKeyNameLen,
                nullptr, // reserved
                nullptr, // no class
                nullptr, // no class
                nullptr  // no last write time
            );
            if (retCode != ERROR_SUCCESS)
            {
                error = platform_error{ retCode, "Cannot enumerate subkeys: RegEnumKeyEx failed." };
            }

            // On success, the ::RegEnumKeyEx API writes the length of the
            // subkey name in the subKeyNameLen output parameter
            // (not including the terminating NUL).
            // So I can build a wstring based on that length.
            subkeyNames.emplace_back(nameBuffer.get(), subKeyNameLen);
        }
    }

    return subkeyNames;
}

std::vector<std::pair<std::wstring, registry_value_types>>
    registry_get_wvalues(const tstring path, int access)
{
    platform_error error;
    auto result = registry_get_wvalues(path, access, error);

    if (error)
        throw error;

    return result;
}

std::vector<std::pair<std::wstring, registry_value_types>> 
    registry_get_wvalues(const tstring path, int access, platform_error& error)
{
    // The value names and types will be stored here
    std::vector<std::pair<std::wstring, registry_value_types>> valueInfo;

    if (detail::AutoHKEY key = detail::_open_key(path, KEY_READ | access, error))
    {
        // Get useful enumeration info, like the total number of values
        // and the maximum length of the value names
        DWORD valueCount = 0;
        DWORD maxValueNameLen = 0;
        LONG retCode = RegQueryInfoKey(
            key,
            nullptr,    // no user-defined class
            nullptr,    // no user-defined class size
            nullptr,    // reserved
            nullptr,    // no subkey count
            nullptr,    // no subkey max length
            nullptr,    // no subkey class length
            &valueCount,
            &maxValueNameLen,
            nullptr,    // no max value length
            nullptr,    // no security descriptor
            nullptr     // no last write time
        );
        if (retCode != ERROR_SUCCESS)
        {
            error = platform_error{
                retCode,
                "RegQueryInfoKey failed while preparing for value enumeration."
            };

            return valueInfo;
        }

        // NOTE: According to the MSDN documentation, the size returned for value name max length
        // does *not* include the terminating NUL, so let's add +1 to take it into account
        // when I allocate the buffer for reading value names.
        maxValueNameLen++;

        // Preallocate a buffer for the value names
        auto nameBuffer = std::make_unique<wchar_t[]>(maxValueNameLen);

        // Reserve room in the vector to speed up the following insertion loop
        valueInfo.reserve(valueCount);

        // Enumerate all the values
        for (DWORD index = 0; index < valueCount; index++)
        {
            // Get the name and the type of the current value
            DWORD valueNameLen = maxValueNameLen;
            DWORD valueType = 0;
            retCode = RegEnumValueW(
                key,
                index,
                nameBuffer.get(),
                &valueNameLen,
                nullptr,    // reserved
                &valueType,
                nullptr,    // no data
                nullptr     // no data size
            );
            if (retCode != ERROR_SUCCESS)
            {
                error = platform_error{ retCode, "Cannot enumerate values: RegEnumValue failed." };

                return valueInfo;
            }

            // On success, the RegEnumValue API writes the length of the
            // value name in the valueNameLen output parameter
            // (not including the terminating NUL).
            // So we can build a wstring based on that.
            valueInfo.emplace_back(
                std::make_pair(
                    std::wstring(nameBuffer.get(), valueNameLen),
                    detail::_conver_type(valueType)
                )
            );
        }
    }

    return valueInfo;
}

} // win
} // util