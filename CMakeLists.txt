cmake_minimum_required(VERSION 3.27)
project(LPAF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DBUILD_DEBUG)

    set(C_FLAGS "-O0 -g3 -Wall -Wextra -Wundef")
    set(CXX_FLAGS "-O0 -g3 -Wall -Wextra -Wundef")
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    add_definitions(-DBUILD_RELEASE)

    # opt for size since clang really likes loop unrolling which clutters up the inst. cache
    set(C_FLAGS "-Os -Wall -Wextra -Wundef")
    set(CXX_FLAGS "-Os -Wall -Wextra -Wundef")
endif ()

set(CMAKE_C_STANDARD 23) # only partial support but there is so much good stuff in that revision
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(framework)
add_subdirectory(tests)
