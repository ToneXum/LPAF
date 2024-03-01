cmake_minimum_required(VERSION 3.27)
project(LPAF)

### VARIABLES

set(APPLICATION_SOURCE
        Main.cpp)

### END VARIABLES

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -Wall)

include_directories("../Framework")
link_directories("$ENV{VULKAN_SDK}/Lib")

if (DEBUG)
    add_definitions(-D _DEBUG)
    set(CMAKE_CXX_FLAGS -O0)

    link_directories("../cmake-build-debug/Framework")
    link_libraries("FrameworkWin64Debug.lib" "vulkan-1.lib")
    add_executable(Test_Application ${APPLICATION_SOURCE})
endif (DEBUG)

if (NOT_DEBUG)
    add_definitions(-D _NDEBUG)
    set(CMAKE_CXX_FLAGS -O3)

    link_directories("../cmake-build-release/Framework")
    link_libraries("FrameworkWin64.lib" "vulkan-1.lib")
    add_executable(Test_Application WIN32 ${APPLICATION_SOURCE})
endif (NOT_DEBUG)
