#if OS_WIN
#   include <WS2tcpip.h>
#   pragma  comment(lib, "ws2_32.lib")
#endif

#include "utility.hpp"
#include "cpu.ipp"
#include "service_win.ipp"
#include "console_win.ipp"
#include "registry_win.ipp"
#include "mini_dump_win.ipp"
#include "platform_win.ipp"
#include "platform_unix.ipp"
#include "platform_util.ipp"
#include "platform_error.ipp"