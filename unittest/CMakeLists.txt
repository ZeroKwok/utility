cmake_minimum_required(VERSION 3.10)

# 是否支持自动连接
option(UTILITY_ENABLE_AUTO_LINK "Enable auto link utility library support" OFF)

project(utility_test)

set(TEST_SOUCES
    string.cpp
    string_util.cpp
    #platform_cpu.cpp 
    platform_util.cpp
    #filesystem_file.cpp 
    #filesystem_path.cpp 
    #common.cpp
    #common_unit.cpp 
    #common_math.cpp
    common_version.cpp
    #common_bytedata.cpp
    #common_encryption.cpp
    )

if(WIN32)
    list(APPEND TEST_SOUCES 
#                process.cpp
#                mini_dump.cpp
                platform_registry.cpp)
endif()

# 测试项目
add_executable(${PROJECT_NAME} ${TEST_SOUCES})

# 连接测试库
target_link_libraries(${PROJECT_NAME} gtest_main)

# 若没有启动自动连接支持
if(NOT UTILITY_ENABLE_AUTO_LINK)

    # 尝试查找是否安装utility, 优先使用安装的utility
    find_package(utility)

    target_link_libraries(${PROJECT_NAME} 
        utility::string 
        utility::filesystem 
        utility::platform 
        utility::common)

    if(MSVC)
        target_link_libraries(${PROJECT_NAME} utility::process)
    endif()

    target_compile_definitions(${PROJECT_NAME} PRIVATE UTILITY_DISABLE_AUTO_LINK)
endif()

# 添加到测试
# gtest_discover_tests(${PROJECT_NAME})