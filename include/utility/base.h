// This file is part of the utility project.
// Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.
//
// v0.1    2018-04
// v0.2    2020-08
// v0.3    2024-05

#ifndef __base_h__
#define __base_h__

// Define platform macro
#if defined(WIN32) || defined(_WIN32)
#   define OS_WIN      1
#elif defined(__APPLE__) && (defined(__i386__) || defined(__ARMEL__))
#   define OS_IOS      1
#elif defined(__APPLE__)
#   define OS_MACOSX   1
#elif defined(__linux__)
#   define OS_LINUX    1
#elif defined(__ANDROID__)
#   define OS_ANDROID  1
#elif defined(__FreeBSD__)
#   define OS_FREEBSD  1
#else
#   error Please add support for your platform in config.h
#endif

// For access to standard POSIXish features, use OS_POSIX instead of a
// more specific macro.
#if defined(OS_MACOSX) || defined(OS_IOS) || defined(OS_LINUX) || defined(OS_ANDROID) || defined(OS_FREEBSD)
#   define OS_POSIX    1
#endif

// Compiler detection.
#if defined(__GNUC__)
#   define COMPILER_GCC 1
#elif defined(_MSC_VER)
#   define COMPILER_MSVC 1
#else
#   error Please add support for your compiler in config.h
#endif

// Processor architecture detection.  For more info on what's defined, see:
// http://msdn.microsoft.com/en-us/library/b0084kay.aspx
// http://www.agner.org/optimize/calling_conventions.pdf
// or with gcc, run: "echo | gcc -E -dM -"
#if defined(_M_X64) || defined(__x86_64__)
#   define ARCH_CPU_X86_FAMILY 1
#   define ARCH_CPU_X86_64     1
#   define ARCH_CPU_64_BITS    1
#elif defined(_M_IX86) || defined(__i386__)
#   define ARCH_CPU_X86_FAMILY 1
#   define ARCH_CPU_X86        1
#   define ARCH_CPU_32_BITS    1
#elif defined(__ARMEL__)
#   define ARCH_CPU_ARM_FAMILY 1
#   define ARCH_CPU_ARMEL      1
#   define ARCH_CPU_32_BITS    1
#   define WCHAR_T_IS_UNSIGNED 1
#else
#   error Please add support for your architecture in config.h
#endif

// GNU libc offers the helpful header <endian.h> which defines
// __BYTE_ORDER
#if defined(__GLIBC__)
#   include <endian.h>
#   if (__BYTE_ORDER == __LITTLE_ENDIAN)
#       define CPU_LITTLE_ENDIAN    1
#   elif (__BYTE_ORDER == __BIG_ENDIAN)
#       define CPU_BIG_ENDIAN   1
#   elif (__BYTE_ORDER == __PDP_ENDIAN)
#       define CPU_PDP_ENDIAN   1
#   else
#       error Unknown machine endianness detected.
#   endif
#   define CPU_BYTE_ORDER __BYTE_ORDER
#elif defined(_BIG_ENDIAN)
#   define CPU_BIG_ENDIAN       1
#   define CPU_BYTE_ORDER       4321
#elif defined(_LITTLE_ENDIAN)
#    define CPU_LITTLE_ENDIAN   1
#    define CPU_BYTE_ORDER      1234
#elif defined(__sparc) || defined(__sparc__) \
   || defined(_POWER) || defined(__powerpc__) \
   || defined(__ppc__) || defined(__hpux) \
   || defined(_MIPSEB) || defined(_POWER) \
   || defined(_MIPSEB) || defined(_POWER) \
   || defined(__s390__)
#   define CPU_BIG_ENDIAN       1
#   define CPU_BYTE_ORDER       4321
#elif defined(__i386__) || defined(__alpha__) \
    || defined(__ia64) || defined(__ia64__) \
    || defined(_M_IX86) || defined(_M_IA64) \
    || defined(_M_ALPHA) || defined(__amd64) \
    || defined(__amd64__) || defined(_M_AMD64) \
    || defined(__x86_64) || defined(__x86_64__) \
    || defined(_M_X64) || defined (__ARMEL__)
#   define CPU_LITTLE_ENDIAN    1
#   define CPU_BYTE_ORDER       1234
#else
#   error Please set up your byte order config.h.
#endif

// Refer: https://github.com/fmtlib/fmt/blob/master/include/fmt/base.h
#ifdef __has_feature
#  define __HAS_FEATURE(x) __has_feature(x)
#else
#  define __HAS_FEATURE(x) 0
#endif
#ifdef __has_include
#  define __HAS_INCLUDE(x) __has_include(x)
#else
#  define __HAS_INCLUDE(x) 0
#endif
#ifdef __has_cpp_attribute
#  define __HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#  define __HAS_CPP_ATTRIBUTE(x) 0
#endif

// _MSVC_LANG
//
// Refer: https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170
// The macro is the integer literal value 201402L by default, or when the /std:c++14 compiler option is specified. 
// The macro is set to 201703L if the /std:c++17 compiler option is specified. 
// The macro is set to 202002L if the /std:c++20 compiler option is specified. 
// It's set to a higher, unspecified value when the /std:c++latest option is specified. 
// Otherwise, the macro is undefined. 
// The _MSVC_LANG macro and /std compiler options are available beginning in Visual Studio 2015 Update 3.

// __cplusplus in Visual Studio
//
// Refer: https://learn.microsoft.com/en-us/cpp/build/reference/zc-cplusplus?view=msvc-170
// The /Zc:__cplusplus compiler option enables the __cplusplus preprocessor macro to report 
// an updated value for recent C++ language standards support. 
// By default, Visual Studio always returns the value 199711L for the __cplusplus preprocessor macro.
// The /Zc:__cplusplus option is available starting in Visual Studio 2017 version 15.7, and is off by default.

// __cplusplus in GCC
// Refer: https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html
//
// the value of the macro is 199711L for the 1998 C++ standard, 201103L for the 2011 C++ standard, 
// 201402L for the 2014 C++ standard, 201703L for the 2017 C++ standard, 202002L for the 2020 C++ standard, 
// 202302L for the 2023 C++ standard, 
// or an unspecified value strictly larger than 202302L for the experimental languages enabled by -std=c++26 and -std=gnu++26.

#ifdef _MSVC_LANG
#    define __CPLUSPLUS _MSVC_LANG
#elif defined(_MSC_VER) && _MSC_VER >= 1600
#    define __CPLUSPLUS 201103L
#else
#    define __CPLUSPLUS __cplusplus
#endif

#if __CPLUSPLUS >= 201103L
#    define HAS_CPP11    1
#endif

#if __CPLUSPLUS >= 201402L
#    define HAS_CPP14    1
#endif

#if __CPLUSPLUS >= 201703L
#    define HAS_CPP17    1
#endif

#if __CPLUSPLUS >= 202002L
#    define HAS_CPP20    1
#endif

#if __CPLUSPLUS >= 202302L
#    define HAS_CPP23    1
#endif

// __GCC_VERSION
#ifdef __GNUC__
#    define __GCC_VERSION_AT_LEAST(x,y) (__GNUC__ > (x) || __GNUC__ == (x) && __GNUC_MINOR__ >= (y))
#    define __GCC_VERSION_AT_MOST(x,y)  (__GNUC__ < (x) || __GNUC__ == (x) && __GNUC_MINOR__ <= (y))
#else
#    define __GCC_VERSION_AT_LEAST(x,y) 0
#    define __GCC_VERSION_AT_MOST(x,y)  0
#endif

// __DEPRECATED
#if __GCC_VERSION_AT_LEAST(3,1)
#    ifndef __DEPRECATED
#        define __DEPRECATED __attribute__ ((__deprecated__))
#    endif
#    define __DEPRECATED_X(text) __attribute__((__deprecated__(text)))
#elif defined(_MSC_VER)
#    define __DEPRECATED __declspec(deprecated)
#    define __DEPRECATED_X(text) __declspec(deprecated(text))
#else
#    define __DEPRECATED
#    define __DEPRECATED_X(text)
#endif

// https://en.cppreference.com/w/cpp/compiler_support.html
#if __GCC_VERSION_AT_LEAST(9,0) || _MSC_VER >= 1922
#   include <version>
#endif

#if COMPILER_MSVC
#   if defined(DEBUG) || defined(_DEBUG)
#      define IS_DEBUG     1
#   endif
#endif

// MSVC 
#if defined(COMPILER_MSVC)

// Microsoft Visual C++各版本的_MSC_VER值定义
// https://zh.wikipedia.org/zh-hans/Microsoft_Visual_C%2B%2B
// https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B
// https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170
// 

                                   // _MSC_VER == 1943 (Visual Studio 2022 version 17.13.6   MSVC++ 14.43)
#   define  __MSVC_144       1940  // _MSC_VER == 1940 (Visual Studio 2022 version 17.10.1   MSVC++ 14.40)

                                   // _MSC_VER == 1939 (Visual Studio 2022 version 17.9.6    MSVC++ 14.39)
                                   // _MSC_VER == 1932 (Visual Studio 2022 version 17.2      MSVC++ 14.32)
                                   // _MSC_VER == 1931 (Visual Studio 2022 version 17.1      MSVC++ 14.31)
#   define  __MSVC_143       1930  // _MSC_VER == 1930 (Visual Studio 2022 RTW (17.0)        MSVC++ 14.30)

                                   // _MSC_VER == 1929 (Visual Studio 2019 Version 16.10/11  MSVC++ 14.29)
                                   // _MSC_VER == 1928 (Visual Studio 2019 Version 16.8/9    MSVC++ 14.28)
                                   // _MSC_VER == 1927 (Visual Studio 2019 Version 16.7      MSVC++ 14.27)
                                   // _MSC_VER == 1921 (Visual Studio 2019 Version 16.1 MSVC++ 14.21)

#   define  __MSVC_142       1920  // _MSC_VER == 1920 (Visual Studio 2019 Version 16.0 MSVC++ 14.20)
                                   // _MSC_VER == 1912 (Visual Studio 2017 version 15.5 MSVC++ 14.12)
                                   // _MSC_VER == 1911 (Visual Studio 2017 version 15.3 MSVC++ 14.11)

#   define  __MSVC_141       1910  // _MSC_VER == 1910 (Visual Studio 2017 version 15.0 MSVC++ 14.1)
#   define  __MSVC_140       1900  // _MSC_VER == 1900 (Visual Studio 2015 version 14.0 MSVC++ 14.0)
#   define  __MSVC_120       1800  // _MSC_VER == 1800 (Visual Studio 2013 version 12.0 MSVC++ 12.0)
#   define  __MSVC_110       1700  // _MSC_VER == 1700 (Visual Studio 2012 version 11.0 MSVC++ 11.0)
#   define  __MSVC_100       1600  // _MSC_VER == 1600 (Visual Studio 2010 version 10.0 MSVC++ 10.0)
#   define  __MSVC_90        1500  // _MSC_VER == 1500 (Visual Studio 2008 version 9.0  MSVC++ 9.0 )
#   define  __MSVC_80        1400  // _MSC_VER == 1400 (Visual Studio 2005 version 8.0  MSVC++ 8.0 )


// 去除Windows.h中携带的min, max宏定义, 且在后面禁止包含
#   ifndef  NOMINMAX
#      define  NOMINMAX
#   endif
#   ifndef WIN32_LEAN_AND_MEAN
#      define  WIN32_LEAN_AND_MEAN 
#   endif
#   ifdef min
#      undef min
#   endif
#   ifdef max
#      undef max
#   endif
#endif // if defined(COMPILER_MSVC)

// SDK Version
#if OS_WIN
#   if defined(_WIN32_WINNT)
#       include <winsdkver.h>
#       include <sdkddkver.h>
#   endif
#endif

#endif // __base_h__
