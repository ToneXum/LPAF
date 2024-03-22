cmake_minimum_required(VERSION 3.27)
project(LPAF)

### VARIABLES

set(FRAMEWORK_SOURCE
        Framework.cpp
        Internals.cpp
        Vulkan.cpp
        OS_Win32.cpp
        OS_Linux.cpp
        OS_MacOS.cpp)

### END VARIABLES

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -Wall)
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(OUT_NAME "lpaf")

include_directories("$ENV{VULKAN_SDK}/Include")

# Debug build
if (DEBUG)
    set(CMAKE_CXX_FLAGS -O0)
    add_library(Framework_Lib STATIC ${FRAMEWORK_SOURCE})
    string(APPEND ${OUT_NAME} "Deb")
endif (DEBUG)

# Optimised release build
if (NOT_DEBUG)
    set(CMAKE_CXX_FLAGS -O3)
    add_library(Framework_Lib STATIC ${FRAMEWORK_SOURCE})
endif (NOT_DEBUG)

# Platform dependant definitions
if (WIN32)
    add_definitions(-D _WINDOWS)
    string(APPEND ${OUT_NAME} "Win")
    set(FILE_SUFFIX ".lib")
endif ()

if (LINUX)
    add_definitions(-D _LINUX)
    string(APPEND ${OUT_NAME} "Lin")
    set(FILE_SUFFIX ".a")
endif ()

if (UNIX AND APPLE)
    add_definitions(-D _MACOS)
    string(APPEND ${OUT_NAME} "Mac")
    set(FILE_SUFFIX ".a")
endif ()

set_target_properties(Framework_Lib PROPERTIES
        OUTPUT_NAME ${OUT_NAME}
        SUFFIX ${FILE_SUFFIX})
