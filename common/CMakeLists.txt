cmake_minimum_required(VERSION 3.10)

project(common CXX)

if(UTILITY_BUILD_SHARED_LIB)
    add_library(${PROJECT_NAME} SHARED impl/common.cpp)
else()
    add_library(${PROJECT_NAME} STATIC impl/common.cpp)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    ${UTILITY_TARGET_COMPILE_DEFINITIONS})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)

if(UTILITY_SUPPORT_BOOST)
    if(UNIX OR NOT UTILITY_BOOST_SUPPORT_AUTOLINK)
        target_link_libraries(${PROJECT_NAME} Boost::thread)
    endif()
endif()

library_regular_naming(${PROJECT_NAME})

# 添加公共头文件
file(GLOB INCLUDE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${INCLUDE_FILES}")

# 设置安装目标
install(TARGETS ${PROJECT_NAME}
    EXPORT utility-common
    ARCHIVE DESTINATION lib                   # 静态库安装路径
    LIBRARY DESTINATION lib                   # 动态库安装路径
    PUBLIC_HEADER DESTINATION include/common) # 头文件安装路径(需要在目标属性里面设置PUBLIC_HEADER)

install(EXPORT utility-common
    FILE utility-commonConfig.cmake           # 包配置文件名
    NAMESPACE utility::                       # 名称空间
    DESTINATION cmake)                        # 文件存储目录

# 安装pdb
install_pdb(${PROJECT_NAME})