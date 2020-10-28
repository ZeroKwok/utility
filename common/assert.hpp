#ifndef assert_h__
#define assert_h__

#include <common/common_cfg.hpp>

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

#if defined(OS_WIN) && !defined(_WIN64)
    __asm int 3; // break point
#endif

    return false;
}

} // detail
} // util

#endif // assert_h__
