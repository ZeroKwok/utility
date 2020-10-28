cmake_minimum_required(VERSION 3.10)

# 是否支持自动连接
option(UTILITY_ENABLE_AUTO_LINK "Enable auto link utility library support" OFF)

project(utility_test)

# 添加GoogleTest支持
include(GoogleTest)

# 查找googletest
find_package(GTest 1.10.0)

# 找不到尝试编译
if(NOT GTest_NOTFOUND)
	add_subdirectory(googletest-1.10.0)
endif()

# 测试项目
add_executable(${PROJECT_NAME} 
	string.cpp
	string_util.cpp
	filesystem.cpp 
	process.cpp 
	common_unit.cpp 
	platform_cpu.cpp)

# 连接测试库
target_link_libraries(${PROJECT_NAME} gtest_main)

# 若没有启动自动连接支持
if(NOT UTILITY_ENABLE_AUTO_LINK)
	target_link_libraries(${PROJECT_NAME} string filesystem platform process common)
	target_compile_definitions(${PROJECT_NAME} PRIVATE UTILITY_DISABLE_AUTO_LINK)
endif()

# 添加到测试
gtest_discover_tests(${PROJECT_NAME})