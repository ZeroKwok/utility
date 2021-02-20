#include "utility.hpp"

#include "cpu.ipp"
#include "platform_util.ipp"
#include "platform_error.ipp"

#if OS_WIN
#   include <WS2tcpip.h>
#   pragma  comment(lib, "ws2_32.lib")

#   include "service_win.ipp"
#   include "console_win.ipp"
#   include "registry_win.ipp"
#   include "mini_dump_win.ipp"
#   include "platform_win.ipp"

#elif OS_POSIX
#   include "platform_unix.ipp"
#endif