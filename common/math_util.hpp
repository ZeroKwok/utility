#ifndef math_util_h__
#define math_util_h__

/*
*   math_util.hpp 
*
*   v0.1 2018-08 by GuoJH
*/

#include <common/common_cfg.hpp>

namespace util {
namespace math {

/*!
 *  ���������������Ƿ����.
 */
UTILITY_FUNCT_DECL bool is_float32_equal(float left, float right);
UTILITY_FUNCT_DECL bool is_float64_equal(double left, double right);

/*!
 *  �������Լ��.
 * 
 *  assert(gcd(27, 15) == 3);
 *  assert(gcd(12, 16) == 4);
 */
UTILITY_FUNCT_DECL float gcd(float a, float b);
UTILITY_FUNCT_DECL double gcd(double a, double b);

/*!
 *  ������С������.
 *
 *  assert(lcm(18, 20) == 120);
 */
UTILITY_FUNCT_DECL float lcm(float a, float b);
UTILITY_FUNCT_DECL double lcm(double a, double b);

} // math
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/math_util.ipp"
#endif

#endif // math_util_h__
