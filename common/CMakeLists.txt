cmake_minimum_required(VERSION 3.10)

project(common CXX)

if(UTILITY_BUILD_SHARED_LIB)
	add_library(${PROJECT_NAME} SHARED impl/common.cpp)
else()
	add_library(${PROJECT_NAME} STATIC impl/common.cpp)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE ${UTILITY_TARGET_COMPILE_DEFINITIONS})

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

# 安装pdb
get_target_property(t_pdb_name ${PROJECT_NAME} COMPILE_PDB_NAME)
get_target_property(t_pdb_name_debug ${PROJECT_NAME} COMPILE_PDB_NAME_DEBUG)
get_target_property(t_pdb_output_directory ${PROJECT_NAME} PDB_OUTPUT_DIRECTORY)

message("${t_pdb_output_directory}/\${CMAKE_INSTALL_CONFIG_NAME}/$<$<CONFIG:Debug>:${t_pdb_name_debug}>$<$<NOT:$<CONFIG:Debug>>:${t_pdb_name}>.pdb")
install(FILES
    "${t_pdb_output_directory}/\${CMAKE_INSTALL_CONFIG_NAME}/$<$<CONFIG:Debug>:${t_pdb_name_debug}>$<$<NOT:$<CONFIG:Debug>>:${t_pdb_name}>.pdb"
    DESTINATION lib
    OPTIONAL)

install(EXPORT utility-common
    FILE utility-commonConfig.cmake           # 包配置文件名
    DESTINATION cmake)                        # 文件存储目录