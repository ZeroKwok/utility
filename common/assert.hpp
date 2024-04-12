#ifndef assert_h__
#define assert_h__

/*
*   assert.hpp
*
*   v0.1 2021-10 by GuoJH
*/

#include <common/common_cfg.h>
#include <string/string_util.h>
#include <platform/console_win.h>

// util_assert
#ifdef _DEBUG
#   define util_assert(expr)  (void)( (!!(expr)) || (util::detail::util_assert_fail(#expr, __FILE__, __LINE__)) )
#else
#   define util_assert(expr)  ((void)0)
#endif

namespace util {
namespace detail {

// util_assert_fail
inline bool util_assert_fail(const char *expr, const char *file, int line)
{
    using namespace util::win;
    util::win::output(util::sformat("Assertion failed[%s], file %s, line %d\n", expr, file, line), 
        STD_OUTPUT, COLOR_RED);

#if ARCH_CPU_X86
#   if OS_WIN
        __asm int 3; // break point
#   else
        __asm__ __volatile__("int 3");
#   endif
#endif

    return false;
}

} // detail
} // util

#endif // assert_h__
