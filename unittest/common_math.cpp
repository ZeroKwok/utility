#include <gtest/gtest.h>
#include <common/math_util.h>

TEST(common_math, is_float_equal)
{
    EXPECT_FALSE(util::math::is_float32_equal(0.01f, 0.009f));
    EXPECT_FALSE(util::math::is_float32_equal(0.00001f, 0.000009f));
    EXPECT_TRUE (util::math::is_float32_equal(0.000009f, 0.000009f));

    EXPECT_FALSE(util::math::is_float64_equal(0.01, 0.009));
    EXPECT_FALSE(util::math::is_float64_equal(0.00001, 0.000009));
    EXPECT_TRUE (util::math::is_float64_equal(0.000009, 0.000009));
    EXPECT_TRUE (util::math::is_float64_equal(0.000009, 0.000009));
}
