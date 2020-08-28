#ifndef config_h__
#define config_h__

/*
*   config.h 
*   
*   v0.1    2018-4 By GuoJH
*   v0.2    2020-8 By GuoJH
*/

// Define platform macro
#if defined(WIN32) || defined(_WIN32)
#   define OS_WIN      1
#elif defined(__APPLE__) && (defined(__i386__) || defined(__ARMEL__))
#   define OS_IOS      1
#elif defined(__APPLE__)
#   define OS_MACOSX   1
#elif defined(__linux__)
#   define OS_LINUX    1
#elif defined(__FreeBSD__)
#   define OS_FREEBSD  1
#else
#   error Please add support for your platform in config.h
#endif

// For access to standard POSIXish features, use OS_POSIX instead of a
// more specific macro.
#if defined(OS_MACOSX) || defined(OS_IOS) || defined(OS_LINUX) || defined(OS_FREEBSD)
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
#       define CPU_LITTLE_ENDIAN
#   elif (__BYTE_ORDER == __BIG_ENDIAN)
#       define CPU_BIG_ENDIAN
#   elif (__BYTE_ORDER == __PDP_ENDIAN)
#       define CPU_PDP_ENDIAN
#   else
#       error Unknown machine endianness detected.
#   endif
#   define CPU_BYTE_ORDER __BYTE_ORDER
#elif defined(_BIG_ENDIAN)
#   define CPU_BIG_ENDIAN
#   define CPU_BYTE_ORDER 4321
#elif defined(_LITTLE_ENDIAN)
#    define CPU_LITTLE_ENDIAN
#    define CPU_BYTE_ORDER 1234
#elif defined(__sparc) || defined(__sparc__) \
   || defined(_POWER) || defined(__powerpc__) \
   || defined(__ppc__) || defined(__hpux) \
   || defined(_MIPSEB) || defined(_POWER) \
   || defined(_MIPSEB) || defined(_POWER) \
   || defined(__s390__)
#   define CPU_BIG_ENDIAN
#   define CPU_BYTE_ORDER 4321
#elif defined(__i386__) || defined(__alpha__) \
    || defined(__ia64) || defined(__ia64__) \
    || defined(_M_IX86) || defined(_M_IA64) \
    || defined(_M_ALPHA) || defined(__amd64) \
    || defined(__amd64__) || defined(_M_AMD64) \
    || defined(__x86_64) || defined(__x86_64__) \
    || defined(_M_X64) || defined (__ARMEL__)
#   define CPU_LITTLE_ENDIAN
#   define CPU_BYTE_ORDER 1234
#else
#   error Please set up your byte order config.h.
#endif

// Microsoft Visual C++各版本的_MSC_VER值定义
#if defined(COMPILER_MSVC)
#   define _MSVC_16_OLDEST  1927   // (Visual Studio 2019) MSVC++ 16.7
#   define _MSVC_16         1920   // (Visual Studio 2019) MSVC++ 16.0
#   define _MSVC_15_OLDEST  1916   // (Visual Studio 2017) MSVC++ 15.9
#   define _MSVC_15         1910   // (Visual Studio 2017) MSVC++ 15.0
#   define _MSVC_14         1900   // (Visual Studio 2015) MSVC++ 14.0
#   define _MSVC_12         1800   // (Visual Studio 2013) MSVC++ 12.0
#   define _MSVC_11         1700   // (Visual Studio 2012) MSVC++ 11.0
#   define _MSVC_10         1600   // (Visual Studio 2010) MSVC++ 10.0
#   define _MSVC_09         1500   // (Visual Studio 2008) MSVC++ 9.0 
#   define _MSVC_08         1400   // (Visual Studio 2005) MSVC++ 8.0 
#   define _MSVC_07         1310   // (Visual Studio 2003) MSVC++ 7.1 
#endif

// noexcept
#if defined(COMPILER_MSVC)
#   if _MSC_VER < _MSVC_15 
#       define noexcept throw()
#   endif
#   include <tchar.h>
#endif

// MSVC 10之前的版本使用自身携带的stdint.h
#if defined(COMPILER_MSVC) && _MSC_VER >= _MSVC_10
#   include <stdint.h>
#else
#   include "msinttypes/stdint.h"
#   ifndef nullptr
#       define nullptr NULL
#   endif
#endif

// The arraysize(arr) macro returns the # of elements in an array arr.
// The expression is a compile-time constant, and therefore can be
// used in defining new arrays, for example.  If you use arraysize on
// a pointer by mistake, you will get a compile-time error.
//
// One caveat is that arraysize() doesn't accept any array of an
// anonymous type or a type defined inside a function.  In these rare
// cases, you have to use the unsafe ARRAYSIZE_UNSAFE() macro below.  This is
// due to a limitation in C++'s template system.  The limitation might
// eventually be removed, but it hasn't happened yet.

// This template function declaration is used in defining arraysize.
// Note that the function doesn't need an implementation, as we only
// use its type.
template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

// That gcc wants both of these prototypes seems mysterious. VC, for
// its part, can't decide which to use (another mystery). Matching of
// template overloads: the final frontier.
#ifndef _MSC_VER
template <typename T, size_t N>
char (&ArraySizeHelper(const T (&array)[N]))[N];
#endif

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

// 去除Windows.h中携带的min, max宏定义, 且在后面禁止包含
#if defined(OS_WIN)
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
#endif

#endif // config_h__
