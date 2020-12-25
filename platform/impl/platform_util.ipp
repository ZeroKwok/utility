#ifdef UTILITY_DISABLE_HEADONLY
#   include "../platform_util.hpp"
#endif

#include <stdint.h>
#include <string>

namespace util {

// Returns true, if this program running on the little-endian CPU.
bool is_little_endian()
{
    uint16_t _16 = 0x00ff;
    uint8_t* _08 = (uint8_t*)&_16;

    return *_08 == 0xff;
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

} // util