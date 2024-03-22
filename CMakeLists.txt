cmake_minimum_required(VERSION 3.27)
project(LPAF)

### CONFIGURATION

# Change this to the directory of your Vulkan SDK
set(ENV{VULKAN_SDK} "../../.SKD/Vulkan")

### END CONFIGURATION

if (DEBUG)
    add_definitions(-D _DEBUG)
endif ()
if (NOT_DEBUG)
    add_definitions(-D NDEBUG)
endif ()

# Actual framework
add_subdirectory("Framework")

# Test project
add_subdirectory("Tests")
