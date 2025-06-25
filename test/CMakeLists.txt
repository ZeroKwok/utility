# This file is part of the utility project.
# Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
#
# For the full copyright and license information, please view the LICENSE
# file that was distributed with this source code.

cmake_minimum_required(VERSION 3.15)

project(utility_test)
find_package(GTest REQUIRED)

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(utility_test ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} utility gtest::gtest GTest::gtest_main)
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
