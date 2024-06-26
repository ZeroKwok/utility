include("${CMAKE_CURRENT_LIST_DIR}/utility-stringConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/utility-commonConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/utility-processConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/utility-platformConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/utility-filesystemConfig.cmake")

get_filename_component(UTILITY_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(UTILITY_PREFIX "${UTILITY_PREFIX}" PATH)
if(UTILITY_PREFIX STREQUAL "/")
  set(UTILITY_PREFIX "")
endif()

set(utility_INCLUDE_DIRS "${UTILITY_PREFIX}/include")
set(utility_LIBRARY_DIRS "${UTILITY_PREFIX}/lib")
set(utility_SOURCE_DIR   "${UTILITY_PREFIX}")
unset(UTILITY_PREFIX)

link_directories(${utility_LIBRARY_DIRS})