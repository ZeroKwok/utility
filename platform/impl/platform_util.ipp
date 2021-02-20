#ifdef UTILITY_DISABLE_HEADONLY
#   include "../platform_util.hpp"
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <iostream>
#include <string/string_conv_easy.hpp>

namespace util {

// Returns true, if this program running on the little-endian CPU.
bool is_little_endian()
{
    uint16_t _16 = 0x00ff;
    uint8_t* _08 = (uint8_t*)&_16;

    return *_08 == 0xff;
}

std::string environment_variable(const char* name)
{
#if defined(_MSC_VER) && _MSC_VER >= 1400
    // getenv is declared as deprecated in VS2005. This function
    // makes use of the new secure getenv function.

    // 
    // https://docs.microsoft.com/en-us/previous-versions/tb2sfw2z(v=vs.140)

    size_t requiredSize = 0;
    std::string buffer;
    getenv_s(&requiredSize, 0, 0, name);
    if (requiredSize == 0)
        return buffer;

    buffer.resize(int(requiredSize));
    getenv_s(&requiredSize, &buffer[0], requiredSize, name);

    // requiredSize includes the terminating null, which we don't want.
    return buffer.c_str();
#else
    if (auto value = ::getenv(name))
        return value;

    return "";
#endif
}

bool set_environment_variable(const std::string& name, const std::string& value)
{
#if defined(_MSC_VER) && _MSC_VER >= 1400
    return _putenv_s(name.c_str(), value.data()) == 0;
#else
    std::string buffer(name);
    if(!value.empty())
    {
        buffer += '=';
        buffer += value;
    }

    char* envVar = strdup(buffer.data());
    int result = putenv(envVar);
    if (result != 0) // error. we have to delete the string.
        free(envVar);
    return result == 0;
#endif
}

std::string format_error(int error_code)
{
#if OS_WIN
    return win::format_error(error_code);
#else
    return posix::format_error(error_code);
#endif
}

std::wstring wformat_error(int error_code)
{
#if OS_WIN
    return win::wformat_error(error_code);
#else
    return posix::wformat_error(error_code);
#endif
}

void output_debug_string(const char* format, ...)
{
#if OS_WIN
    va_list list = nullptr;
    va_start(list, format);

    std::string message(_vscprintf(format, list) + 1, 0);
    vsprintf_s(&message[0], message.capacity(), format, list);

    if (message[message.size() - 1] == '\0')
        message.erase(message.begin() + (message.size() - 1));

    ::OutputDebugStringA((message + "\r\n").c_str());

    std::cout << message << std::endl;

    va_end(list);
#else
    va_list list;
    va_start(list, format);

    std::string message(vsnprintf(nullptr, 0, format, list) + 1, 0);

    // These functions do not call the va_end macro. 
    // Because they invoke the va_arg macro, the value of ap is 
    // undefined after the call.
    // https://linux.die.net/man/3/vsnprintf
    va_start(list, format);

    size_t size = vsnprintf(&message[0], message.size(), format, list);

    std::cout << message << std::endl;
#endif
}

void output_debug_string(const std::string& message)
{
#if OS_WIN
    ::OutputDebugStringA((message + "\r\n").c_str());
#endif

    std::cout << message << std::endl;
}

void output_debug_string(const std::wstring& message)
{
#if OS_WIN
    ::OutputDebugStringW((msg + L"\r\n").c_str());
#endif

    std::wcout << message << std::endl;
}

} // util