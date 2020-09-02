cmake_minimum_required(VERSION 3.10)

project(platform CXX)

if(UTILITY_BUILD_SHARED_LIB)
	add_library(${PROJECT_NAME} SHARED impl/platform.cpp)
else()
	add_library(${PROJECT_NAME} STATIC impl/platform.cpp)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE ${UTILITY_TARGET_COMPILE_DEFINITIONS})

library_regular_naming(${PROJECT_NAME})