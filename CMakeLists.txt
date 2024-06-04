# This file is part of the utility project.
# Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
#
# For the full copyright and license information, please view the LICENSE
# file that was distributed with this source code.

cmake_minimum_required(VERSION 3.13)

project(utility VERSION 0.3.0 LANGUAGES CXX)

# Build unit tests
option(UTILITY_BUILD_TEST "Enable building unit tests" ON)

# Build shared libraries
option(UTILITY_BUILD_SHARED_LIB "Enable building shared libraries" ON)

# Link runtime statically
option(UTILITY_BUILD_STATIC_RUNTIME "Enable linking with runtime statically" OFF)

# Enable Boost support
option(UTILITY_SUPPORT_BOOST "Enable Boost support" ON)

# Enable Qt support
option(UTILITY_SUPPORT_QT "Enable Qt support" OFF)

#
# 配置项目环境
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(UTILITY_TARGET_COMPILE_DEFINITIONS "")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(cmake_utility)

if(WIN32)
    # 若Windows SDK 没有启动支持WinXP SP1则默认兼容到 Windows 7 SP1
    if(UTILITY_SUPPORT_WINXP)
        list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS _WIN32_WINNT=0x0502)
    else()
        list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS _WIN32_WINNT=0x0601)
    endif()
    list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS UTILITY_SUPPORT_BSTR=1)
endif()

if(UTILITY_SUPPORT_BOOST)
    list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS UTILITY_SUPPORT_BOOST=1)
endif()

if(UTILITY_SUPPORT_QT)
    list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS UTILITY_SUPPORT_QT=1)
endif()

if(UTILITY_BUILD_SHARED_LIB)
    list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS UTILITY_BUILD_SHARED_LIB=1)
else()
    list(APPEND UTILITY_TARGET_COMPILE_DEFINITIONS UTILITY_BUILD_STATIC_LIB=1)
endif()

# 静态运行时
if(MSVC AND UTILITY_BUILD_STATIC_RUNTIME)
    foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    endforeach(flag_var)
endif()

# 重定向生成目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/lib")
set(CMAKE_PDB_OUTPUT_DIRECTORY     "${CMAKE_CURRENT_SOURCE_DIR}/bin/lib")

if(UTILITY_SUPPORT_BOOST)
    if(MSVC)
        set(UTILITY_BOOST_SUPPORT_AUTOLINK ON)
        set(Boost_USE_STATIC_LIBS ON)
        set(Boost_USE_STATIC_RUNTIME ${UTILITY_BUILD_STATIC_RUNTIME})
    endif()

    if (NOT Boost_INCLUDE_DIRS)
        find_package(Boost REQUIRED filesystem thread)
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

file(GLOB INCLUDE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/utility/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/utility/*.hpp")
file(GLOB SOURCE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# 填充配置
set(UTILITY_BUILD_STRING \${UTILITY_BUILD_STRING})
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/utility/config.h.in" 
    "${CMAKE_BINARY_DIR}/config.h.tmp")

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/include/utility/config.h
    COMMAND python ./git-build-gen.py "${CMAKE_BINARY_DIR}/config.h.tmp" "${CMAKE_CURRENT_SOURCE_DIR}/include/utility/config.h"
    DEPENDS "${CMAKE_BINARY_DIR}/config.h.tmp" "${CMAKE_CURRENT_SOURCE_DIR}/git-build-gen.py"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/utility/config.h)

if(MSVC)
    # 设置utf-8编译 & resource.rc"
    add_compile_options(/utf-8)
    if(UTILITY_BUILD_SHARED_LIB)
        utility_fix_project_version()
        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/src/resource.rc.in" 
            "${CMAKE_BINARY_DIR}/resource.rc.tmp")
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/resource.rc
            COMMAND python ./git-build-gen.py "${CMAKE_BINARY_DIR}/resource.rc.tmp" "${CMAKE_CURRENT_SOURCE_DIR}/src/resource.rc"
            DEPENDS "${CMAKE_BINARY_DIR}/resource.rc.tmp" "${CMAKE_CURRENT_SOURCE_DIR}/git-build-gen.py"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        list(APPEND SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/resource.rc)
    endif()
endif()

if(UTILITY_BUILD_SHARED_LIB)
    add_library(${PROJECT_NAME} SHARED ${INCLUDE_FILES} ${SOURCE_FILES} )
    target_compile_definitions(${PROJECT_NAME} PRIVATE UTILITY_BUILD_SHARED_LIB)
else()
    add_library(${PROJECT_NAME} STATIC ${INCLUDE_FILES} ${SOURCE_FILES})
    target_compile_definitions(${PROJECT_NAME} PUBLIC  UTILITY_USING_STATIC_LIB)
endif()

if(UTILITY_SUPPORT_QT)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    ${UTILITY_TARGET_COMPILE_DEFINITIONS})

set_target_properties(${PROJECT_NAME} PROPERTIES 
    PUBLIC_HEADER "${INCLUDE_FILES}"
    DEBUG_POSTFIX "d")
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)

#
# 安装
install(EXPORT ${PROJECT_NAME} FILE ${PROJECT_NAME}Config.cmake DESTINATION cmake)
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib 
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/utility)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE DESTINATION ./ OPTIONAL)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md DESTINATION ./ OPTIONAL)

# 生成版本文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/utilityConfigVersion.cmake"
  VERSION ${utility_VERSION}
  COMPATIBILITY AnyNewerVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/utilityConfigVersion.cmake" DESTINATION cmake)

# If the compiler is MSVC, install the PDB file
if(MSVC)
    if(UTILITY_BUILD_SHARED_LIB)
        install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION bin OPTIONAL)
    else()
        utility_install_pdb(${PROJECT_NAME})
    endif()
endif()

#
# 单元测试
if(UTILITY_BUILD_TEST)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()

#
# 测试 CMake 工具
if (UTILITY_BUILD_TEST AND UTILITY_BUILD_TEST_CMAKE_UTILITY)
    message("--------------------- Test CMake Utility ---------------------")
    utility_print_target_properties(${PROJECT_NAME})
    message("---")
    utility_print_current_variables()
    message("------------------------ Test Finished -----------------------")
endif()