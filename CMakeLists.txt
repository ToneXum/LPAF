cmake_minimum_required(VERSION 3.27)
project(LPAF)

### CONFIGURATION

# Change this to the directory of your Vulkan SDK
set(ENV{VULKAN_SDK} "../../.SKD/Vulkan")

### END CONFIGURATION

# Actual framework
add_subdirectory("Framework")

# Test project
add_subdirectory("Tests")
