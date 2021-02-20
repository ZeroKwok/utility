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

set(TEST_SOUCES
    string.cpp
    string_util.cpp
    platform_cpu.cpp 
    platform_util.cpp
    filesystem_file.cpp 
    filesystem_path.cpp 
    common_unit.cpp 
    common_math.cpp
    common_encryption.cpp)

if(WIN32)
    list(APPEND TEST_SOUCES 
                process.cpp
                mini_dump.cpp
                platform_registry.cpp)
endif()

# 测试项目
add_executable(${PROJECT_NAME} ${TEST_SOUCES})

# 连接测试库
target_link_libraries(${PROJECT_NAME} gtest_main)

# 若没有启动自动连接支持
if(NOT UTILITY_ENABLE_AUTO_LINK)
    # 尝试查找是否安装utility, 优先使用安装的utility
    # 因安装版本采用了名称空间故这里需要区分
    find_package(utility)

    if(utility_FOUND)
        target_link_libraries(${PROJECT_NAME} 
            utility::string 
            utility::filesystem 
            utility::platform 
            utility::common)

        if(MSVC)
            target_link_libraries(${PROJECT_NAME} utility::process)
        endif()
    else()
        target_link_libraries(${PROJECT_NAME} 
            string 
            platform 
            filesystem 
            common)

        if(MSVC)
            target_link_libraries(${PROJECT_NAME} process)
        endif()
    endif()
    target_compile_definitions(${PROJECT_NAME} PRIVATE UTILITY_DISABLE_AUTO_LINK)
endif()

# 添加到测试
gtest_discover_tests(${PROJECT_NAME})