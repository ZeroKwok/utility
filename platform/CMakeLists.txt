cmake_minimum_required(VERSION 3.10)

project(platform CXX)

if(UTILITY_BUILD_SHARED_LIB)
	add_library(${PROJECT_NAME} SHARED impl/platform.cpp)
else()
	add_library(${PROJECT_NAME} STATIC impl/platform.cpp)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE ${UTILITY_TARGET_COMPILE_DEFINITIONS})

library_regular_naming(${PROJECT_NAME})

file(GLOB INCLUDE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${INCLUDE_FILES}")

install(TARGETS ${PROJECT_NAME}
    EXPORT utility-platform
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib 
    PUBLIC_HEADER DESTINATION include/platform)

install(EXPORT utility-platform
    FILE utility-platformConfig.cmake
    NAMESPACE utility::
    DESTINATION cmake)