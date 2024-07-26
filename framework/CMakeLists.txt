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
        framework.c
        framework.h
        internal.c
        internal.h
        framework-linux.c
        internal-linux.c
        linux.h
)
add_library(lpafLib STATIC ${FRAMEWORK_SOURCE})
set_target_properties(lpafLib PROPERTIES
        OUTPUT_NAME "${LIB_NAME}$<$<CONFIG:Debug>:-D>"
        PREFIX ""
        SUFFIX "${FILE_SUFFIX}"
)
