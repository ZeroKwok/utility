#include <gtest/gtest.h>
#include <platform/cpu.h>

TEST(platform, cpu)
{
#if defined(ARCH_CPU_X86_FAMILY)
        // Retrieve the CPU information.
        util::cpu cpu;

        EXPECT_TRUE(0 != cpu.id());
        EXPECT_TRUE("" != cpu.id_string());

#if defined(OS_WIN) && defined(ARCH_CPU_32_BITS)
        EXPECT_TRUE(cpu.has_mmx());

        // Execute an MMX instruction.
        __asm emms;

        if (cpu.has_sse())
        {
            // Execute an SSE instruction.
            __asm xorps xmm0, xmm0;
        }

        if (cpu.has_sse2())
        {
            // Execute an SSE 2 instruction.
            __asm psrldq xmm0, 0;
        }

        if (cpu.has_sse3())
        {
            // Execute an SSE 3 instruction.
            __asm addsubpd xmm0, xmm0;
        }

        if (cpu.has_ssse3())
        {
            // Execute a Supplimental SSE 3 instruction.
            __asm psignb xmm0, xmm0;
        }

#if _MSC_VER > _MSVC_08
        if (cpu.has_sse41())
        {
            // Execute an SSE 4.1 instruction.
            __asm pmuldq xmm0, xmm0;
        }

        if (cpu.has_sse42())
        {
            // Execute an SSE 4.2 instruction.
            __asm crc32 eax, eax;
        }
#endif

#elif defined(OS_POSIX) && defined(__x86_64__)
        EXPECT_TRUE(cpu.has_mmx());

        // Execute an MMX instruction.
        __asm__ __volatile__("emms\n" : : : "mm0");

        if (cpu.has_sse())
        {
            // Execute an SSE instruction.
            __asm__ __volatile__("xorps %%xmm0, %%xmm0\n" : : : "xmm0");
        }

        if (cpu.has_sse2())
        {
            // Execute an SSE 2 instruction.
            __asm__ __volatile__("psrldq $0, %%xmm0\n" : : : "xmm0");
        }

        if (cpu.has_sse3())
        {
            // Execute an SSE 3 instruction.
            __asm__ __volatile__("addsubpd %%xmm0, %%xmm0\n" : : : "xmm0");
        }

        if (cpu.has_ssse3())
        {
            // Execute a Supplimental SSE 3 instruction.
            __asm__ __volatile__("psignb %%xmm0, %%xmm0\n" : : : "xmm0");
        }

        if (cpu.has_sse41())
        {
            // Execute an SSE 4.1 instruction.
            __asm__ __volatile__("pmuldq %%xmm0, %%xmm0\n" : : : "xmm0");
        }

        if (cpu.has_sse42())
        {
            // Execute an SSE 4.2 instruction.
            __asm__ __volatile__("crc32 %%eax, %%eax\n" : : : "eax");
        }
#endif
#endif
}


