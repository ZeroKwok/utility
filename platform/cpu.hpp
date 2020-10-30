#ifndef cpul_h__
#define cpul_h__

/*
*	cpu.hpp
*   
*   2019-12 by GuoJH
*/

#include <string>
#include <platform/platform_cfg.hpp>

namespace util {

/*
*   Query information about the processor.
*/
class UTILITY_CLASS_DECL cpu
{
public:
    // Constructor
    UTILITY_MEMBER_DECL cpu()
        : _type      (0)
        , _family    (0)
        , _model     (0)
        , _stepping  (0)
        , _ext_model (0)
        , _ext_family(0)
        , _has_mmx   (false)
        , _has_sse   (false)
        , _has_sse2  (false)
        , _has_sse3  (false)
        , _has_ssse3 (false)
        , _has_sse41 (false)
        , _has_sse42 (false)
        , _cpu_id    (0)
        , _cpu_vendor("unknown")
    {
        initialize();
    }

    /*
    *   Accessors for CPU information.
    */
    UTILITY_MEMBER_DECL int type()      const { return _type; }
    UTILITY_MEMBER_DECL int family()    const { return _family; }
    UTILITY_MEMBER_DECL int model()     const { return _model; }
    UTILITY_MEMBER_DECL int stepping()  const { return _stepping; }
    UTILITY_MEMBER_DECL int has_mmx()   const { return _has_mmx; }
    UTILITY_MEMBER_DECL int has_sse()   const { return _has_sse; }
    UTILITY_MEMBER_DECL int has_sse2()  const { return _has_sse2; }
    UTILITY_MEMBER_DECL int has_sse3()  const { return _has_sse3; }
    UTILITY_MEMBER_DECL int has_ssse3() const { return _has_ssse3; }
    UTILITY_MEMBER_DECL int has_sse41() const { return _has_sse41; }
    UTILITY_MEMBER_DECL int has_sse42() const { return _has_sse42; }
    UTILITY_MEMBER_DECL int extended_model()  const { return _ext_model; }
    UTILITY_MEMBER_DECL int extended_family() const { return _ext_family; }
    UTILITY_MEMBER_DECL const std::string& vendor_name() const { return _cpu_vendor; }

    /*
    *   Returns a windows-style CPU Id.
    *   Equate to $wmic cpu get processor id
    */
    UTILITY_MEMBER_DECL uint64_t    id() const { return _cpu_id; }
    UTILITY_MEMBER_DECL std::string id_string() const;

private:
    /*
    *   Query the processor for CPUID information.
    */
    UTILITY_MEMBER_DECL void initialize();

    int  _type;              // process type
    int  _family;            // family of the processor
    int  _model;             // model of processor
    int  _stepping;          // processor revision number
    int  _ext_model;         
    int  _ext_family;        
    bool _has_mmx;           //MMX
    bool _has_sse;           //SSE
    bool _has_sse2;          //SSE2
    bool _has_sse3;          //SSE3
    bool _has_ssse3;         //SSSE3
    bool _has_sse41;         //SSE4.1
    bool _has_sse42;         //SSE4.2
    uint64_t _cpu_id;        //CPU Id
    std::string _cpu_vendor;
};

#if defined(ARCH_CPU_X86_FAMILY)
#    ifndef COMPILER_MSVC
#       if defined(__pic__) && defined(__i386__)
            UTILITY_MEMBER_DECL void __cpuid  (int cpu_info[4], int info_type);
            UTILITY_MEMBER_DECL void __cpuidex(int cpu_info[4], int info_type, int info_index);
#       else
            UTILITY_MEMBER_DECL void __cpuid  (int cpu_info[4], int info_type);
            UTILITY_MEMBER_DECL void __cpuidex(int cpu_info[4], int info_type, int info_index);
#       endif // __i386___
#   endif // COMPILER_MSVC
#endif // ARCH_CPU_X86_FAMILY

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/cpu.ipp"
#endif

#endif // cpul_h__
