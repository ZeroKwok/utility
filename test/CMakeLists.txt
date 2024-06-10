# This file is part of the utility project.
# Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
#
# For the full copyright and license information, please view the LICENSE
# file that was distributed with this source code.

cmake_minimum_required(VERSION 3.13)

enable_testing()

find_package(GTest CONFIG REQUIRED)

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(utility_test ${SOURCE_FILES})
target_link_libraries(
    utility_test PRIVATE
    utility
    GTest::gtest_main
    )
    
include(GoogleTest)
gtest_discover_tests(utility_test)
