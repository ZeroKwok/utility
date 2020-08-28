cmake_minimum_required(VERSION 3.10)

project(string CXX)

if(UTILITY_BUILD_SHARED_LIB)
	add_library(${PROJECT_NAME} SHARED impl/string.cpp)
else()
	add_library(${PROJECT_NAME} STATIC impl/string.cpp)
endif()

set_boost_style_naming(${PROJECT_NAME})

message("ssss= ${UTILITY_TARGET_COMPILE_DEFINITIONS}")

target_compile_definitions(${PROJECT_NAME} PRIVATE ${UTILITY_TARGET_COMPILE_DEFINITIONS})