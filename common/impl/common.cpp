#include "utility.hpp"

// VS2010�л���� timeGetTime �Ҳ�����ʶ�������
#ifdef WIN32_LEAN_AND_MEAN
#   undef  WIN32_LEAN_AND_MEAN 
#endif

#include "time_util.ipp"

#ifndef WIN32_LEAN_AND_MEAN
#   define  WIN32_LEAN_AND_MEAN 
#endif

#include "unit.ipp"
#include "bytedata.ipp"
#include "math_util.ipp"
#include "encryption.ipp"
#include "thread_interrupt.ipp"
