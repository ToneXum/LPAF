cmake_minimum_required(VERSION 3.27)
project(LPAF)

### LANGUAGE CONFIG
if (DEBUG MATCHES true)
    add_definitions(-DBUILD_DEBUG)

    set(C_FLAGS "-Wall -Wextra -Wundef -O0 -g3")
    set(CXX_FLAGS "-Wall -Wextra -Wundef -O0 -g3")
elseif (DEBUG MATCHES false)
    add_definitions(-DBUILD_RELEASE)

    set(C_FLAGS "-Wall -Wextra -Wundef -O3")
    set(CXX_FLAGS "-Wall -Wextra -Wundef -O3")
endif ()

set(CMAKE_C_STANDARD 23) # only partial support but there is so much good stuff in that revision
set(CMAKE_CXX_STANDARD 23)

### LIB BUILD SETTINGS
if (WIN32)
    add_definitions(-DPLATFORM_WINDOWS)
    set(LIB_NAME "lpafWin")
    set(FILE_SUFFIX ".lib")
elseif (LINUX)
    add_definitions(-DPLATFORM_LINUX)
    set(LIB_NAME "lpafLin")
    set(FILE_SUFFIX ".a")
elseif (APPLE AND UNIX)
    add_definitions(-DPLATFORM_MACOS)
    set(LIB_NAME "lpafMac")
    set(FILE_SUFFIX ".a")
endif ()

set(FRAMEWORK_SOURCE
        framework/framework.c
        framework/framework.h
        framework/win32.c
        framework/win32.h
        framework/internal.c
        framework/internal.h
)
add_library(lpafLib STATIC ${FRAMEWORK_SOURCE})
set_target_properties(lpafLib PROPERTIES
        OUTPUT_NAME "${LIB_NAME}$<$<CONFIG:Debug>:-D>"
        PREFIX ""
        SUFFIX "${FILE_SUFFIX}"
)

### TEST PROGRAM
set(APPLICATION_SOURCE
        tests/tests.c
        tests/tests.h
        tests/main.c
)
add_executable(lpafTest ${APPLICATION_SOURCE})
target_include_directories(lpafTest PUBLIC framework/)
target_link_libraries(lpafTest $<TARGET_OBJECTS:lpafLib>)
