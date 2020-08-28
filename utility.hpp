#ifndef utility_h__
#define utility_h__

/*
*   utility.hpp 
*
*   2020-08-28 By GuoJH
*/

#if defined(UTILITY_BUILD_SHARED_LIB) || defined(UTILITY_BUILD_STATIC_LIB) ||\
    defined(UTILITY_USING_SHARED_LIB) || defined(UTILITY_USING_STATIC_LIB)
#   ifndef UTILITY_DISABLE_HEADONLY
#       define UTILITY_DISABLE_HEADONLY 1
#   endif
#endif

#if defined(UTILITY_BUILD_SHARED_LIB)                 // build shared lib
#   define UTILITY_CLASS_DECL __declspec(dllexport)   
#   define UTILITY_FUNCT_DECL __declspec(dllexport)
#   define UTILITY_MEMBER_DECL
#elif defined(UTILITY_USING_SHARED_LIB)               // using shared lib
#   define UTILITY_CLASS_DECL __declspec(dllimport)   
#   define UTILITY_FUNCT_DECL __declspec(dllimport)
#   define UTILITY_MEMBER_DECL
#else
#   if !defined(UTILITY_DISABLE_HEADONLY)             // head-only
#       define UTILITY_CLASS_DECL
#       define UTILITY_FUNCT_DECL  inline
#       define UTILITY_MEMBER_DECL inline
#   else                                              // build/using static lib, not head-only 
#       define UTILITY_CLASS_DECL
#       define UTILITY_FUNCT_DECL
#       define UTILITY_MEMBER_DECL
#   endif
#endif

#include "config.h"

// Auto link
#if defined(UTILITY_USING_SHARED_LIB) || defined(UTILITY_USING_STATIC_LIB)
#   ifdef OS_WIN
#   ifdef UTILITY_USING_SHARED_LIB
#       define AUTO_LINK_DYN_LINK  1
#   endif
#       define UTILITY_ENABLE_AUTO_LINK 1
#   endif
#endif

// Defined WINNT version.
#if defined(WIN32) || defined(_WIN32)
#   ifndef _WIN32_WINNT
#       define _WIN32_WINNT 0x0501 // Windows XP
#   endif
#endif

#endif // utility_h__
