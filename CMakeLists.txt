# This file is part of the utility project.
# Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
#
# For the full copyright and license information, please view the LICENSE
# file that was distributed with this source code.

cmake_minimum_required(VERSION 3.15)

project(utility VERSION 0.3.0)

option(UTILITY_BUILD_TEST           "Build unit tests" ON)
option(UTILITY_BUILD_SHARED_LIB     "Build shared libraries" ON)
option(UTILITY_BUILD_STATIC_RUNTIME "Link with static runtime" OFF)
option(UTILITY_SUPPORT_BOOST        "Enable Boost support" ON)
option(UTILITY_SUPPORT_QT           "Enable Qt support" OFF)

message(STATUS "Building Utility v${utility_VERSION}")
message(STATUS "Build configuration:")
message(STATUS "  Build tests ............ ${UTILITY_BUILD_TEST}")
message(STATUS "  Build shared libs ...... ${UTILITY_BUILD_SHARED_LIB}")
message(STATUS "  Static runtime ......... ${UTILITY_BUILD_STATIC_RUNTIME}")
message(STATUS "  Boost support .......... ${UTILITY_SUPPORT_BOOST}")
message(STATUS "  Qt support ............. ${UTILITY_SUPPORT_QT}")

#
# 配置项目环境
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(UTILITY_PUBLIC_DEFINITIONS "")
set(UTILITY_PRIVATE_DEFINITIONS "")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

include(cmake_utility)

if(WIN32)
    list(APPEND UTILITY_PUBLIC_DEFINITIONS _WIN32_WINNT=0x0601)
    list(APPEND UTILITY_PUBLIC_DEFINITIONS UTILITY_SUPPORT_BSTR=1)
endif()

if(UTILITY_SUPPORT_BOOST)
    list(APPEND UTILITY_PUBLIC_DEFINITIONS UTILITY_SUPPORT_BOOST=1)
endif()

if(UTILITY_SUPPORT_QT)
    list(APPEND UTILITY_PUBLIC_DEFINITIONS UTILITY_SUPPORT_QT=1)
endif()

if(UTILITY_BUILD_SHARED_LIB)
    list(APPEND UTILITY_PRIVATE_DEFINITIONS UTILITY_BUILD_SHARED_LIB=1)
else()
    list(APPEND UTILITY_PUBLIC_DEFINITIONS  UTILITY_USING_STATIC_LIB=1)
    list(APPEND UTILITY_PRIVATE_DEFINITIONS UTILITY_BUILD_STATIC_LIB=1)
endif()

# 静态运行时
if(MSVC)
    add_compile_options(/utf-8)
    if(UTILITY_BUILD_STATIC_RUNTIME)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    # 重定向生成目录
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/lib")
    set(CMAKE_PDB_OUTPUT_DIRECTORY     "${CMAKE_CURRENT_SOURCE_DIR}/bin/lib")
endif()

if(UTILITY_SUPPORT_BOOST)
    if(MSVC)
        set(Boost_USE_STATIC_LIBS ON)
        set(Boost_USE_STATIC_RUNTIME ${UTILITY_BUILD_STATIC_RUNTIME})
    endif()
    find_package(Boost REQUIRED)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    add_compile_definitions(BOOST_LIB_DIAGNOSTIC=1)
endif()

if(UTILITY_SUPPORT_QT)
    find_package(Qt5 REQUIRED Core)
endif()

file(GLOB_RECURSE INCLUDE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/utility/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/utility/*.hpp")
file(GLOB_RECURSE SOURCE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# 填充配置
set(PROJECT_BUILD_STRING \${PROJECT_BUILD_STRING})
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/utility/config.h.in" 
    "${CMAKE_BINARY_DIR}/config.h.tmp")

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/include/utility/config.h
    COMMAND python ./scripts/git-build-gen.py "${CMAKE_BINARY_DIR}/config.h.tmp" "${CMAKE_CURRENT_SOURCE_DIR}/include/utility/config.h"
    DEPENDS "${CMAKE_BINARY_DIR}/config.h.tmp" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/git-build-gen.py"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/utility/config.h)

if(MSVC AND UTILITY_BUILD_SHARED_LIB)
    utility_fix_project_version()
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/src/resource.rc.in" 
        "${CMAKE_BINARY_DIR}/resource.rc.tmp")
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/resource.rc
        COMMAND python ./scripts/git-build-gen.py "${CMAKE_BINARY_DIR}/resource.rc.tmp" "${CMAKE_CURRENT_SOURCE_DIR}/src/resource.rc"
        DEPENDS "${CMAKE_BINARY_DIR}/resource.rc.tmp" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/git-build-gen.py"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    list(APPEND SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/resource.rc)
endif()

if(UTILITY_BUILD_SHARED_LIB)
    add_library(${PROJECT_NAME} SHARED ${INCLUDE_FILES} ${SOURCE_FILES} )
else()
    add_library(${PROJECT_NAME} STATIC ${INCLUDE_FILES} ${SOURCE_FILES})
endif()

if(UTILITY_SUPPORT_QT)
    target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core)
endif()

target_compile_definitions(${PROJECT_NAME} 
    PUBLIC  ${UTILITY_PUBLIC_DEFINITIONS}
    PRIVATE ${UTILITY_PRIVATE_DEFINITIONS})

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