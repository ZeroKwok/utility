#include <utility.hpp>
#ifdef UTILITY_DISABLE_HEADONLY
#   include "../string_conv.hpp"
#endif

#if OS_WIN
#   include "string_conv_win.ipp"
#else
#   include "string_conv_unix.ipp"
#endif