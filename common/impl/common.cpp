#include "utility.hpp"

// VS2010中会出现 timeGetTime 找不到标识符的情况
#ifdef WIN32_LEAN_AND_MEAN
#   undef  WIN32_LEAN_AND_MEAN 
#endif

#include "time_util.ipp"

#ifndef WIN32_LEAN_AND_MEAN
#   define  WIN32_LEAN_AND_MEAN 
#endif

#include "unit.ipp"
#include "bytedata.ipp"
