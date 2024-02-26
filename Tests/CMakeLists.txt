cmake_minimum_required(VERSION 3.27)
project(LPAF)

### VARIABLES

set(APPLICATION_SOURCE
        Main.cpp)

### END VARIABLES

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -Wall)

include_directories("../Framework")

if (DEBUG)
    add_definitions(-D _DEBUG)
    set(CMAKE_CXX_FLAGS -O0)

    link_directories("../.done")
    link_libraries("FrameworkWin64Debug.lib")
    add_executable(Test_Application ${APPLICATION_SOURCE})
endif (DEBUG)

if (NOT_DEBUG)
    add_definitions(-D _NDEBUG)
    set(CMAKE_CXX_FLAGS -O3)

    link_directories("../.done")
    link_libraries("FrameworkWin64.lib")
    add_executable(Test_Application WIN32 ${APPLICATION_SOURCE})
endif (NOT_DEBUG)
