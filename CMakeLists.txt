cmake_minimum_required(VERSION 3.27)
project(LPAF)

### CONFIGURATION

# Change this to the directory of your Vulkan SDK
set(ENV{VULKAN_SDK} "../../.SKD/Vulkan")

# Set to anything else than "yeah" to not build the tests
set(BUILD_TEST_PROGRAM "yeah")

### END CONFIGURATION

set(C_FLAGS "-Wall -Wextra -Wundef")
set(CXX_FLAGS "-Wall -Wextra -Wundef")

if (DEBUG)
    add_definitions(-D _DEBUG)

    string(APPEND ${C_FLAGS} " -O0 -g3")
    string(APPEND ${CXX_FLAGS} " -O0 -g3")
else ()
    add_definitions(-D NDEBUG)

    string(APPEND ${C_FLAGS} " -O3")
    string(APPEND ${CXX_FLAGS} " -O3")
endif ()

set(CMAKE_C_STANDARD 23) # only partial support but there is so much good stuff in that revision
set(CMAKE_C_FLAGS ${C_FLAGS})

set(CMAKE_CXX_STANDARD 20) # fully supported
set(CMAKE_CXX_FLAGS ${CXX_FLAGS})

set(OUT_NAME "lpaf")
if (WIN32)
    add_definitions(-D_WINDOWS)
    string(APPEND ${OUT_NAME} "Win")
    set(FILE_SUFFIX ".lib")
    enable_language("RC")
elseif (LINUX)
    add_definitions(-D_LINUX)
    string(APPEND ${OUT_NAME} "Lin")
    set(FILE_SUFFIX ".a")
elseif (APPLE AND UNIX)
    add_definitions(-D_MACOS)
    string(APPEND ${OUT_NAME} "Mac")
    set(FILE_SUFFIX ".a")
endif ()

set(CMAKE_STATIC_LIBRARY_PREFIX "")

set(FRAMEWORK_SOURCE
        framework/framework.c
        framework/win32.c
)
add_library(lpaf STATIC ${FRAMEWORK_SOURCE})
set_target_properties(lpaf PROPERTIES
        OUTPUT_NAME ${OUT_NAME}
        SUFFIX ${FILE_SUFFIX}
)

if (BUILD_TEST_PROGRAM MATCHES "yeah") # build test program, does so by default
    set(APPLICATION_SOURCE
            tests/tests.c
            tests/main.c
    )
    add_executable(lpafTest ${APPLICATION_SOURCE})
    target_include_directories(lpafTest PUBLIC framework/)
    target_link_libraries(lpafTest $<TARGET_OBJECTS:lpaf>)
    set_target_properties(PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/
    )
endif ()