cmake_minimum_required(VERSION 3.10)

project(process CXX)

if(UTILITY_BUILD_SHARED_LIB)
	add_library(${PROJECT_NAME} SHARED impl/process.cpp)
else()
	add_library(${PROJECT_NAME} STATIC impl/process.cpp)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE ${UTILITY_TARGET_COMPILE_DEFINITIONS})

library_regular_naming(${PROJECT_NAME})

file(GLOB INCLUDE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${INCLUDE_FILES}")

install(TARGETS ${PROJECT_NAME}
    EXPORT utility-process
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib 
    PUBLIC_HEADER DESTINATION include/process)

install(EXPORT utility-process
    FILE utility-processConfig.cmake
    NAMESPACE utility::
    DESTINATION cmake)