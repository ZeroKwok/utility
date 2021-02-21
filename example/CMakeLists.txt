cmake_minimum_required(VERSION 3.10)

# 是否支持自动连接
option(UTILITY_ENABLE_AUTO_LINK "Enable auto link utility library support" OFF)

project(simple)

find_package(utility REQUIRED)

add_executable(${PROJECT_NAME} simple.cpp)

# 若没有启动自动连接支持
if(NOT UTILITY_ENABLE_AUTO_LINK)
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