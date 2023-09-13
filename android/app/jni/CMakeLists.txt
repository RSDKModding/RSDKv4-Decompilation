cmake_minimum_required(VERSION 3.7)
project(RSDKv4)

set(curdir ${CMAKE_CURRENT_LIST_DIR})
set(android 1)

# message(FATAL_ERROR ${curdir})
file(GLOB children ${curdir}/*)

foreach(child ${children})
    if(EXISTS ${child}/CMakeLists.txt)
        add_subdirectory(${child})
    endif()
endforeach()
