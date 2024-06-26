cmake_policy(SET CMP0074 NEW)
cmake_minimum_required(VERSION 3.13)

project(utility VERSION 0.2.2 LANGUAGES CXX)

# Build unit tests
option(UTILITY_BUILD_TEST "Enable building unit tests" ON)

# Build shared libraries
option(UTILITY_BUILD_SHARED_LIB "Enable building shared libraries" OFF)

# Link runtime statically
option(UTILITY_BUILD_STATIC_RUNTIME "Enable linking with runtime statically" OFF)

# Enable Boost support
option(UTILITY_SUPPORT_BOOST "Enable Boost support" ON)

# Enable Qt support
option(UTILITY_SUPPORT_QT "Enable Qt support" OFF)

# Enable WinXP compatibility
option(UTILITY_SUPPORT_WINXP "Enable WinXP compatibility" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(UTILITY_TARGET_COMPILE_DEFINITIONS "")
if(WIN32)
    # 若Windows SDK 没有启动支持WinXP SP1则默认兼容到 Windows 7 SP1
    if(UTILITY_SUPPORT_WINXP)
        list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS _WIN32_WINNT=0x0502)
    else()
        list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS _WIN32_WINNT=0x0601)
    endif()
    list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS UTILITY_SUPPORT_BSTR)
endif()

if(UTILITY_SUPPORT_BOOST)
    list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS UTILITY_SUPPORT_BOOST)
endif()

if(UTILITY_SUPPORT_QT)
    list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS UTILITY_SUPPORT_QT)
endif()

if(UTILITY_BUILD_SHARED_LIB)
    list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS UTILITY_BUILD_SHARED_LIB)
else()
    list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS UTILITY_BUILD_STATIC_LIB)
endif()

# 静态运行时
if(MSVC AND UTILITY_BUILD_STATIC_RUNTIME)
    foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    endforeach(flag_var)
endif()

# 如果编译器支持则启用c++17特性
if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++17" _cpp_17_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++17")
    endif()
endif()

# 重定向生成目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_PDB_OUTPUT_DIRECTORY     "${CMAKE_BINARY_DIR}/lib")

# 设置utf-8编译
if(MSVC)
    add_compile_options(/utf-8)
endif()

# 设置为根目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 添加为连接目录
link_directories("${CMAKE_BINARY_DIR}/lib")

# cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(UTILITY_SUPPORT_BOOST)
    if(MSVC)
        set(UTILITY_BOOST_SUPPORT_AUTOLINK ON)
        set(Boost_USE_STATIC_LIBS ON)
        set(Boost_USE_STATIC_RUNTIME ${UTILITY_BUILD_STATIC_RUNTIME})
    endif()
	
	if (NOT Boost_INCLUDE_DIRS)
		find_package(Boost REQUIRED filesystem thread serialization)
	else()
		message("Has been defined: Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}")
	endif()
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    add_compile_definitions(BOOST_LIB_DIAGNOSTIC=1)
endif()

if(UTILITY_SUPPORT_QT)
    find_package(Qt5 REQUIRED Core)
endif()

# regular naming
include(cmake_debug)
include(internal_util)
include(library_regular_naming)

# 填充配置
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/utility.hpp.in" 
               "${CMAKE_CURRENT_SOURCE_DIR}/utility.hpp")

# 子目录
add_subdirectory(string)
add_subdirectory(platform)
add_subdirectory(filesystem)
add_subdirectory(common)
add_subdirectory(process)

# 单元测试
if(UTILITY_BUILD_TEST)
    # enable_testing()

    # 添加GoogleTest支持
    include(GoogleTest)
    find_package(GTest 1.10.0)

    # 找不到尝试编译
    if(NOT GTest_NOTFOUND)
        if(UTILITY_BUILD_STATIC_RUNTIME)
            set(gtest_force_shared_crt OFF)
        else()
            set(gtest_force_shared_crt ON)
        endif()
        # 参考 googletest:CMakeLists.txt
        message("gtest_force_shared_crt=${gtest_force_shared_crt}")
        add_subdirectory(3rd/googletest-1.10.0)
    endif()

    add_subdirectory(unittest)
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
install(FILES
    msinttypes/stdint.h
    msinttypes/inttypes.h
  DESTINATION
    include/msinttypes)

# 安装CMake配置文件
install(
  FILES
    cmake/utilityConfig.cmake
    cmake/library_regular_naming.cmake
  DESTINATION
    cmake
  COMPONENT
    Devel)

# 生成版本文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/utilityConfigVersion.cmake"
  VERSION ${utility_VERSION}
  COMPATIBILITY AnyNewerVersion)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/utilityConfigVersion.cmake" DESTINATION cmake)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/internal_util.cmake" DESTINATION cmake)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_debug.cmake" DESTINATION cmake)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/README.md" DESTINATION .)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/unittest DESTINATION . OPTIONAL)