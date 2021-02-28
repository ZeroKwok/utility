#ifdef UTILITY_DISABLE_HEADONLY
#   include "../platform_unix.h"
#endif

#if OS_POSIX

#include <errno.h>
#include <string.h>
#include <string/string_conv_easy.hpp>

namespace util{
namespace posix {

// 返回errno的错误描述
std::string format_error(int error_code)
{
    return strerror(error_code);
}

std::wstring wformat_error(int error_code)
{
    return conv::easy::_2wstr(strerror(error_code));
}

} // posix
} // util

#endif // OS_POSIX