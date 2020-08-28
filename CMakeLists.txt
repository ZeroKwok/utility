cmake_minimum_required(VERSION 3.10)

project(utility CXX)

# 是否支持Qt
option(UTILITY_SUPPORT_QT "Enable Qt support" OFF)

# 是否编译动态库
option(UTILITY_BUILD_SHARED_LIB "Enable build shared libraries" OFF)

# 是否启用静态运行时
option(UTILITY_ENABLE_STATIC_RUNTIME "Enable link with runtime statically" OFF)

# 初始化UTILITY_TARGET_COMPILE_DEFINITIONS
set(UTILITY_TARGET_COMPILE_DEFINITIONS UTILITY_SUPPORT_BSTR)

if(UTILITY_SUPPORT_QT)
	list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS UTILITY_SUPPORT_QT)
endif()

if(UTILITY_BUILD_SHARED_LIB)
	list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS UTILITY_BUILD_SHARED_LIB)
else()
	list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS UTILITY_BUILD_STATIC_LIB)
endif()

# 静态运行时
if(MSVC AND UTILITY_ENABLE_STATIC_RUNTIME)
	foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
		string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
	endforeach(flag_var)
endif()

# 设置为根目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 3rd modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# naming
include(boost_style_naming)

# 子目录
add_subdirectory(string)