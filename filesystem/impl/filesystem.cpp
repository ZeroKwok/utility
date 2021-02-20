#include "utility.hpp"
#include "path_util.ipp"
#include "file_util.ipp"

#if OS_WIN
#   include "file_win.ipp"
#elif OS_POSIX
#   include "file_unix.ipp"
#endif
