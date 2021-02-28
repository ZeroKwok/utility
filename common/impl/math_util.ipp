#ifdef UTILITY_DISABLE_HEADONLY
#   include "../math_util.h"
#endif

#include <math.h>

namespace util {
namespace math {
namespace detail {

// From The Google C++ Testing Framework (Google Test)
#include "gtest_float.hpp"

template<class _Type>
inline _Type _gcd(_Type a, _Type b)
{
    if (a == b)
        return a;
    else if (a > b)
        return _gcd(a - b, b);
    else
        return _gcd(b - a, a);
}

template<class _Type>
inline _Type _lcm(_Type a, _Type b)
{
    return a * b / _gcd(a, b);
}

} // detail

bool is_float32_equal(float left, float right)
{
    detail::Float lhs(left), rhs(right);

    return lhs.AlmostEquals(rhs);
}

bool is_float64_equal(double left, double right)
{
    detail::Double lhs(left), rhs(right);

    return lhs.AlmostEquals(rhs);
}

float gcd(float a, float b)
{
    return detail::_gcd(a, b);
}

double gcd(double a, double b)
{
    return detail::_gcd(a, b);
}

float lcm(float a, float b)
{
    return detail::_lcm(a, b);
}

double lcm(double a, double b)
{
    return detail::_lcm(a, b);
}

} // math
} // util