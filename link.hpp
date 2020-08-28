/*************************************************************************

link.hpp - From modified by the: boost\config\auto_link.hpp

2020-01-13 by GuoJH

USAGE:
~~~~~~

Before including this header you must define one or more of define the following macros:

AUTO_LINK_LIB_NAME:           Required: A string containing the basename of the library,
                              for example boost_regex.
AUTO_LINK_LIB_TOOLSET:        Optional: the base name of the toolset.
AUTO_LINK_DYN_LINK:           Optional: when set link to dll rather than static library.
AUTO_LINK_LIB_DIAGNOSTIC:     Optional: when set the header will print out the name
                              of the library selected (useful for debugging).
AUTO_LINK_NOMANGLE:           Specifies that we should link to AUTO_LINK_LIB_NAME.lib,
                              rather than a mangled-name version.

These macros will be undef'ed at the end of the header, further this header
has no include guards - so be sure to include it only once from your library!

Algorithm:
~~~~~~~~~~

Libraries for Borland and Microsoft compilers are automatically
selected here, the name of the lib is selected according to the following
formula:

AUTO_LINK_LIB_PREFIX
   + AUTO_LINK_LIB_NAME
   + "_"
   + AUTO_LINK_LIB_TOOLSET
   + AUTO_LINK_LIB_THREAD_OPT
   + AUTO_LINK_LIB_RT_OPT
   + AUTO_LINK_LIB_ARCH_AND_MODEL_OPT

These are defined as:

AUTO_LINK_LIB_PREFIX:     "lib" for static libraries otherwise "".

AUTO_LINK_LIB_NAME:       The base name of the lib ( for example boost_regex).

AUTO_LINK_LIB_TOOLSET:    The compiler toolset name (vc6, vc7, bcb5 etc).

AUTO_LINK_LIB_THREAD_OPT: "-mt" for multithread builds, otherwise nothing.

AUTO_LINK_LIB_RT_OPT:     A suffix that indicates the runtime library used,
                          contains one or more of the following letters after
                          a hyphen:
                          
                          s      static runtime (dynamic if not present).
                          d      debug build (release if not present).

AUTO_LINK_LIB_ARCH_AND_MODEL_OPT: The architecture and address model
                              (-x32 or -x64 for x86/32 and x86/64 respectively)

AUTO_LINK_LIB_VERSION:    The Boost version, in the form x_y, for version x.y.
 

***************************************************************************/

#ifndef AUTO_LINK_MSVC
#  define AUTO_LINK_MSVC _MSC_VER
#endif

#ifndef AUTO_LINK_STRINGIZE
#  define AUTO_LINK_STRINGIZE(X) AUTO_LINK_DO_STRINGIZE(X)
#  define AUTO_LINK_DO_STRINGIZE(X) #X
#endif

#ifndef AUTO_LINK_LIB_NAME
#  error "Macro AUTO_LINK_LIB_NAME not set (internal error)"
#endif

//
// error check:
//
#if defined(__MSVC_RUNTIME_CHECKS) && !defined(_DEBUG)
#  pragma message("Using the /RTC option without specifying a debug runtime will lead to linker errors")
#  pragma message("Hint: go to the code generation options and switch to one of the debugging runtimes")
#  error "Incompatible build options"
#endif
//
// select toolset if not defined already:
//
#ifndef AUTO_LINK_LIB_TOOLSET
#  if defined(AUTO_LINK_MSVC) && (AUTO_LINK_MSVC < 1200)
// Note: no compilers before 1200 are supported
#    error "Note: no compilers before 1200 are supported"

#  elif defined(AUTO_LINK_MSVC) && (AUTO_LINK_MSVC < 1300)

#    ifdef UNDER_CE
// eVC4:
#      define AUTO_LINK_LIB_TOOLSET "evc4"
#    else
// vc6:
#      define AUTO_LINK_LIB_TOOLSET "vc6"
#    endif

#  elif defined(AUTO_LINK_MSVC) && (AUTO_LINK_MSVC < 1310)

// vc7:
#    define AUTO_LINK_LIB_TOOLSET "vc7"

#  elif defined(AUTO_LINK_MSVC) && (AUTO_LINK_MSVC < 1400)

// vc71:
#    define AUTO_LINK_LIB_TOOLSET "vc71"

#  elif defined(AUTO_LINK_MSVC) && (AUTO_LINK_MSVC < 1500)

// vc80:
#    define AUTO_LINK_LIB_TOOLSET "vc80"

#  elif defined(AUTO_LINK_MSVC) && (AUTO_LINK_MSVC < 1600)

// vc90:
#    define AUTO_LINK_LIB_TOOLSET "vc90"

#  elif defined(AUTO_LINK_MSVC) && (AUTO_LINK_MSVC < 1700)

// vc10:
#    define AUTO_LINK_LIB_TOOLSET "vc100"

#  elif defined(AUTO_LINK_MSVC) && (AUTO_LINK_MSVC < 1800)

// vc11:
#    define AUTO_LINK_LIB_TOOLSET "vc110"

#  elif defined(AUTO_LINK_MSVC) && (AUTO_LINK_MSVC < 1900)

// vc12:
#    define AUTO_LINK_LIB_TOOLSET "vc120"

#  elif defined(AUTO_LINK_MSVC) && (AUTO_LINK_MSVC < 1910)

// vc14:
#    define AUTO_LINK_LIB_TOOLSET "vc140"

#  elif defined(AUTO_LINK_MSVC)

// vc14.1:
#    define AUTO_LINK_LIB_TOOLSET "vc141"

#  elif defined(__BORLANDC__)

// CBuilder 6:
#    define AUTO_LINK_LIB_TOOLSET "bcb"

#  elif defined(__ICL)

// Intel C++, no version number:
#    define AUTO_LINK_LIB_TOOLSET "iw"

#  elif defined(__MWERKS__) && (__MWERKS__ <= 0x31FF )

// Metrowerks CodeWarrior 8.x
#    define AUTO_LINK_LIB_TOOLSET "cw8"

#  elif defined(__MWERKS__) && (__MWERKS__ <= 0x32FF )

// Metrowerks CodeWarrior 9.x
#    define AUTO_LINK_LIB_TOOLSET "cw9"

#  endif
#endif // AUTO_LINK_LIB_TOOLSET

//
// select thread opt:
//
#if defined(_MT) || defined(__MT__)
#  define AUTO_LINK_LIB_THREAD_OPT "-mt"
#else
#  define AUTO_LINK_LIB_THREAD_OPT
#endif

#if defined(_MSC_VER) || defined(__MWERKS__)
#  ifdef _DLL
#    if defined(_DEBUG)
#      define AUTO_LINK_LIB_RT_OPT "-d"
#    else
#      define AUTO_LINK_LIB_RT_OPT
#    endif
#  else
#    if defined(_DEBUG)
#      define AUTO_LINK_LIB_RT_OPT "-sd"
#    else
#      define AUTO_LINK_LIB_RT_OPT "-s"
#    endif
#  endif
#endif

//
// AUTO_LINK_LIB_ARCH_AND_MODEL_OPT
//

#if defined( _M_IX86 )
#  define AUTO_LINK_LIB_ARCH_AND_MODEL_OPT "-x32"
#elif defined( _M_X64 )
#  define AUTO_LINK_LIB_ARCH_AND_MODEL_OPT "-x64"
#elif defined( _M_ARM )
#  define AUTO_LINK_LIB_ARCH_AND_MODEL_OPT "-a32"
#elif defined( _M_ARM64 )
#  define AUTO_LINK_LIB_ARCH_AND_MODEL_OPT "-a64"
#endif

//
// select linkage opt:
//
#if (defined(_DLL) || defined(_RTLDLL)) && defined(AUTO_LINK_DYN_LINK)
#  define AUTO_LINK_LIB_PREFIX
#elif defined(AUTO_LINK_DYN_LINK)
#  error "Mixing a dll library with a static runtime is a really bad idea..."
#else
#  define AUTO_LINK_LIB_PREFIX "lib"
#endif

#ifndef AUTO_LINK_LIB_TOOLSET
#  error "Macro AUTO_LINK_LIB_TOOLSET not set (internal error)"
#endif

#ifndef AUTO_LINK_LIB_THREAD_OPT
#  error "Macro AUTO_LINK_LIB_THREAD_OPT not set (internal error)"
#endif

#ifndef AUTO_LINK_LIB_RT_OPT
#  error "Macro AUTO_LINK_LIB_RT_OPT not set (internal error)"
#endif

#ifndef AUTO_LINK_LIB_ARCH_AND_MODEL_OPT
#  error "Macro AUTO_LINK_LIB_ARCH_AND_MODEL_OPT not set (internal error)"
#endif

//
// now include the lib:
//
#if defined(AUTO_LINK_LIB_NAME) \
    && defined(AUTO_LINK_LIB_PREFIX) \
    && defined(AUTO_LINK_LIB_TOOLSET) \
    && defined(AUTO_LINK_LIB_THREAD_OPT) \
    && defined(AUTO_LINK_LIB_RT_OPT) \
    && defined(AUTO_LINK_LIB_ARCH_AND_MODEL_OPT)

#if defined(AUTO_LINK_NOMANGLE)
#  pragma comment(lib, AUTO_LINK_STRINGIZE(AUTO_LINK_LIB_NAME) ".lib")
#  ifdef AUTO_LINK_LIB_DIAGNOSTIC
#     pragma message ("Linking to lib file: " AUTO_LINK_STRINGIZE(AUTO_LINK_LIB_NAME) ".lib")
#  endif
#else
#  pragma comment(lib, AUTO_LINK_LIB_PREFIX AUTO_LINK_STRINGIZE(AUTO_LINK_LIB_NAME) "-" AUTO_LINK_LIB_TOOLSET AUTO_LINK_LIB_THREAD_OPT AUTO_LINK_LIB_RT_OPT AUTO_LINK_LIB_ARCH_AND_MODEL_OPT ".lib")
#  ifdef AUTO_LINK_LIB_DIAGNOSTIC
#     pragma message ("Linking to lib file: " AUTO_LINK_LIB_PREFIX AUTO_LINK_STRINGIZE(AUTO_LINK_LIB_NAME) "-" AUTO_LINK_LIB_TOOLSET AUTO_LINK_LIB_THREAD_OPT AUTO_LINK_LIB_RT_OPT AUTO_LINK_LIB_ARCH_AND_MODEL_OPT ".lib")
#  endif
#endif

#else
#  error "some required macros where not defined (internal logic error)."
#endif

//
// finally undef any macros we may have set:
//
#if defined(AUTO_LINK_LIB_NAME)
#  undef AUTO_LINK_LIB_NAME
#endif
// Don't undef this one: it can be set by the user and should be the 
// same for all libraries:
//#if defined(AUTO_LINK_LIB_TOOLSET)
//#  undef AUTO_LINK_LIB_TOOLSET
//#endif
#if defined(AUTO_LINK_LIB_PREFIX)
#  undef AUTO_LINK_LIB_PREFIX
#endif
#if defined(AUTO_LINK_LIB_THREAD_OPT)
#  undef AUTO_LINK_LIB_THREAD_OPT
#endif
#if defined(AUTO_LINK_LIB_RT_OPT)
#  undef AUTO_LINK_LIB_RT_OPT
#endif
#if defined(AUTO_LINK_LIB_ARCH_AND_MODEL_OPT)
#  undef AUTO_LINK_LIB_ARCH_AND_MODEL_OPT
#endif
#if defined(AUTO_LINK_DYN_LINK)
#  undef AUTO_LINK_DYN_LINK
#endif