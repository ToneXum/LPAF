cmake_minimum_required(VERSION 3.27)
project(LPAF)

### VARIABLES

set(FRAMEWORK_SOURCE
        Framework.cpp
        Internals.cpp
        Vulkan.cpp)

### END VARIABLES

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -Wall)
set(CMAKE_STATIC_LIBRARY_PREFIX "")

include_directories("$ENV{VULKAN_SDK}/Include")
add_library(Vulkan_Lib STATIC IMPORTED)
set_target_properties(Vulkan_Lib PROPERTIES IMPORTED_LOCATION $ENV{VULKAN_SDK}/Lib/vulkan-1.lib)

# Debug build
if (DEBUG)
    add_definitions(-D _DEBUG)
    set(CMAKE_CXX_FLAGS -O0)

    add_library(Framework_Lib STATIC ${FRAMEWORK_SOURCE})

    if (WIN32)
        set(FILE_NAME "FrameworkWin64Debug")
    endif ()

    if (LINUX)
        set(FILE_NAME "FrameworkLinux64Debug")
    endif ()

    if (UNIX AND APPLE)
        set(FILE_NAME "FrameworkMacOS64Debug")
    endif ()
endif (DEBUG)

# Optimised release build
if (NOT_DEBUG)
    add_definitions(-D _NDEBUG)
    set(CMAKE_CXX_FLAGS -O3)

    add_library(Framework_Lib STATIC ${FRAMEWORK_SOURCE})

    if (WIN32)
        set(FILE_NAME "FrameworkWin64")
    endif ()

    if (LINUX)
        set(FILE_NAME "FrameworkLinux64")
    endif ()

    if (UNIX AND APPLE)
        set(FILE_NAME "FrameworkMacOS64")
    endif ()
endif (NOT_DEBUG)

# Platform dependant definitions
if (WIN32)
    add_definitions(-D _WINDOWS)
    set(FILE_SUFFIX ".lib")
endif ()

if (LINUX)
    add_definitions(-D _LINUX)
    add_definitions(-D _UNIX)

    set(FILE_SUFFIX ".a")
endif ()

if (UNIX AND APPLE)
    add_definitions(-D _MACOS)

    set(FILE_SUFFIX ".a")
endif ()

set_target_properties(Framework_Lib PROPERTIES
        OUTPUT_NAME ${FILE_NAME}
        SUFFIX ${FILE_SUFFIX})
