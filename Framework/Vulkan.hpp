// MIT License
//
// Copyright(c) 2023 ToneXum
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

//STL
#include <vector>

// Vulkan SDK
#include "vulkan/vulkan.h"

#define VUL_EC(x) { VkResult res = x; if (res) { v::CreateVulkanError(__LINE__, res, __func__); } }

namespace v
{
class VulkanState
{
public:
    VulkanState() = default;
    ~VulkanState() = default;

    // Make it singleton
    VulkanState(const VulkanState&) = delete;
    VulkanState(const VulkanState&&) = delete;
    VulkanState operator=(const VulkanState&) = delete;
    VulkanState operator=(const VulkanState&&) = delete;

    VkInstance vkInstance{};
    VkPhysicalDevice physicalDevice{};
    VkDevice device{};

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT debugMessenger{};
    const std::vector<const char*> validationLayers
    {
        "VK_LAYER_KHRONOS_validation"
    };
#endif // _DEBUG

    const std::vector<const char*> extension
    {
#ifdef _DEBUG
        "VK_EXT_debug_utils",
#endif // _DEBUG

        "VK_KHR_surface",
        "VK_KHR_win32_surface",
    };
};

void InitialiseVulkan();

void UnInitializeVulkan();

VkPhysicalDevice ChooseBestPhysicalDevice(const std::vector<VkPhysicalDevice>& dev);

// Vulkan error creation
void CreateVulkanError(int line, int code, const char* func);

#ifdef _DEBUG
// Called when a validation layer is invoked
VkBool32 VKAPI_CALL ValidationDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
    VkDebugUtilsMessageTypeFlagsEXT msgType,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData
);
#endif // _DEBUG

namespace p
{
    // This is a proxy function
    VkResult CreateDebugUtilsMessengerExt(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger
    );

    // This is a proxy function
    void DestroyDebugUtilsMessengerExt(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator
    );
} // end namespace p
} // end namespace v
