#ifdef UTILITY_DISABLE_HEADONLY
#   include "../cpu_util.hpp"
#endif

#include <string/string_util.hpp>

#if defined(ARCH_CPU_X86_FAMILY)
#   if defined(COMPILER_MSVC)
#       include <intrin.h>
#   endif
#endif

namespace util {

#if defined(ARCH_CPU_X86_FAMILY)
#ifndef COMPILER_MSVC
#if defined(__pic__) && defined(__i386__)

void __cpuid(int cpu_info[4], int info_type)
{
    __asm__ volatile (
        "mov %%ebx, %%edi\n"
        "cpuid\n"
        "xchg %%edi, %%ebx\n"
        : "=a"(cpu_info[0]), "=D"(cpu_info[1]), "=c"(cpu_info[2]), "=d"(cpu_info[3])
        : "a"(info_type)
        );
}

void __cpuidex(int cpu_info[4], int info_type, int info_index)
{
    __asm__ volatile (
        "mov %%ebx, %%edi\n"
        "cpuid\n"
        "xchg %%edi, %%ebx\n"
        : "=a"(cpu_info[0]), "=D"(cpu_info[1]), "=c"(cpu_info[2]), "=d"(cpu_info[3])
        : "a"(info_type), "c"(info_index)
        );
}
#else
void __cpuid(int cpu_info[4], int info_type)
{
    __asm__ volatile (
        "cpuid \n\t"
        : "=a"(cpu_info[0]), "=b"(cpu_info[1]), "=c"(cpu_info[2]), "=d"(cpu_info[3])
        : "a"(info_type)
        );
}

void __cpuidex(int cpu_info[4], int info_type, int info_index)
{
    __asm__ volatile (
        "cpuid \n\t"
        : "=a"(cpu_info[0]), "=b"(cpu_info[1]), "=c"(cpu_info[2]), "=d"(cpu_info[3])
        : "a"(info_type), "c"(info_index)
        );
}
#endif
#endif  // COMPILER_MSVC
#endif  // ARCH_CPU_X86_FAMILY

void cpu::initialize()
{
#if defined(ARCH_CPU_X86_FAMILY)
    int cpu_info[4] = {-1};
    char cpu_string[0x20] = {0};

    // __cpuid with an InfoType argument of 0 returns the number of
    // valid Ids in CPUInfo[0] and the CPU identification string in
    // the other three array elements. The CPU identification string is
    // not in linear order. The code below arranges the information
    // in a human readable form.
    //
    // More info can be found here:
    // http://msdn.microsoft.com/en-us/library/hskdteyh.aspx
    __cpuid(cpu_info, 0);
    int num_ids = cpu_info[0];
    *(reinterpret_cast<int*>(cpu_string)) = cpu_info[1];
    *(reinterpret_cast<int*>(cpu_string+4)) = cpu_info[3];
    *(reinterpret_cast<int*>(cpu_string+8)) = cpu_info[2];

    // Interpret CPU feature information.
    if (num_ids > 0)
    {
        __cpuid(cpu_info, 1);
        _stepping   = cpu_info[0] & 0xf;
        _model      = ((cpu_info[0] >> 4) & 0xf) + ((cpu_info[0] >> 12) & 0xf0);
        _family     = (cpu_info[0] >> 8) & 0xf;
        _type       = (cpu_info[0] >> 12) & 0x3;
        _ext_model  = (cpu_info[0] >> 16) & 0xf;
        _ext_family = (cpu_info[0] >> 20) & 0xff;
        _cpu_vendor = cpu_string;
        _has_mmx    = (cpu_info[3] & 0x00800000) != 0;
        _has_sse    = (cpu_info[3] & 0x02000000) != 0;
        _has_sse2   = (cpu_info[3] & 0x04000000) != 0;
        _has_sse3   = (cpu_info[2] & 0x00000001) != 0;
        _has_ssse3  = (cpu_info[2] & 0x00000200) != 0;
        _has_sse41  = (cpu_info[2] & 0x00080000) != 0;
        _has_sse42  = (cpu_info[2] & 0x00100000) != 0;
        _cpu_id     = util::sformat("%08X%08X", cpu_info[3], cpu_info[0]);
    }
#endif
}

} // util
