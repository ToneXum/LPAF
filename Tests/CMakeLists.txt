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
link_libraries("lpaf.lib" "vulkan-1.lib" "ws2_32")

if (DEBUG)
    set(CMAKE_CXX_FLAGS -O0)
    link_directories("../cmake-build-debug/Framework")
    add_executable(Test_Application ${APPLICATION_SOURCE})
endif (DEBUG)

if (NOT_DEBUG)
    set(CMAKE_CXX_FLAGS -O3)
    link_directories("../cmake-build-release/Framework")
    add_executable(Test_Application WIN32 ${APPLICATION_SOURCE})
endif (NOT_DEBUG)

set_target_properties(Test_Application PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "../../.runtime" )
