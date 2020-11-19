cmake_minimum_required(VERSION 3.10)

project(utility CXX)

# 是否编译单元测试
option(UTILITY_BUILD_TEST "Enable build unit test" ON)

# 是否支持Qt
option(UTILITY_SUPPORT_QT "Enable Qt support" OFF)

# 是否编译动态库
option(UTILITY_BUILD_SHARED_LIB "Enable build shared libraries" OFF)

# 是否启用静态运行时
option(UTILITY_ENABLE_STATIC_RUNTIME "Enable link with runtime statically" ON)

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

# 重定向生成目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

if(UTILITY_SUPPORT_QT)
    find_package(Qt5 REQUIRED Widgets)
endif()

# 设置为根目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 添加为连接目录
link_directories("${CMAKE_BINARY_DIR}/lib")

# 3rd modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# regular naming
include(library_regular_naming)

# 子目录
add_subdirectory(string)
add_subdirectory(process)
add_subdirectory(platform)
add_subdirectory(filesystem)
add_subdirectory(common)

# 单元测试
if(UTILITY_BUILD_TEST)
	enable_testing()
	add_subdirectory(test)
endif()

# 安装公共头文件
install(
  FILES
    config.h
    link.hpp
    prefix.hpp
    suffix.hpp
    utility.hpp
  DESTINATION
    include)

# msinttypes
install(
  FILES
    msinttypes/stdint.h
    msinttypes/inttypes.h
  DESTINATION
    include/msinttypes)

# 安装CMake配置文件
install(
  FILES
    cmake/utilityConfig.cmake
  DESTINATION
    cmake
  COMPONENT
    Devel)