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

#include "Vulkan.hpp"
#include "Internals.hpp"

#ifdef _WINDOWS
#include "vulkan/vulkan_win32.h"
#endif

void v::InitialiseVulkan()
{
    // do setup for validation layers
#ifdef _DEBUG
    // make sure that all layers requested are present and enabled
    unsigned int layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    bool allLayersFound = true;
    for (const char* layerName : i::GetState()->vulkan.validationLayers) // loops through the layers we want
    {
        bool found = false;
        for (const VkLayerProperties& layerProperties : availableLayers) // loop through the layers we have
        {
            if (strcmp(layerName, layerProperties.layerName) == 0) // bitmask, 0 means strings are equal
            {
                found = true;
                break;
            }
        }
        if (!found) allLayersFound = false;
    }

    if (!allLayersFound) FRAMEWORK_ERR("Not all validation layers requested where found.")

    VkDebugUtilsMessengerCreateInfoEXT messenger{};
    messenger.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    messenger.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    messenger.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    messenger.pfnUserCallback = v::ValidationDebugCallback;
    messenger.pUserData = nullptr;
#endif // _DEBUG

    VkApplicationInfo appInf{};
    appInf.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInf.pApplicationName = "LPAF Renderer";
    appInf.apiVersion = VK_MAKE_API_VERSION(1, 1, 0, 0);
    appInf.pEngineName = "No Engine";
    appInf.engineVersion = VK_MAKE_API_VERSION(1, 1, 0, 0);
    appInf.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInf{};
    createInf.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInf.pApplicationInfo = &appInf;
#ifdef _DEBUG
    createInf.enabledLayerCount = static_cast<unsigned int>(i::GetState()->vulkan.validationLayers.size());
    createInf.ppEnabledLayerNames = i::GetState()->vulkan.validationLayers.data();
    createInf.pNext = &messenger;
#endif // _DEBUG
    createInf.enabledExtensionCount = static_cast<unsigned int>(i::GetState()->vulkan.extension.size());
    createInf.ppEnabledExtensionNames = i::GetState()->vulkan.extension.data();

    VUL_EC(vkCreateInstance(&createInf, nullptr, &i::GetState()->vulkan.vkInstance))
    i::Log(L"A Vulkan instance was created", i::LlInfo);

#ifdef _DEBUG
    VUL_EC(v::p::CreateDebugUtilsMessengerExt(i::GetState()->vulkan.vkInstance,
                                             &messenger, nullptr, &i::GetState()->vulkan.debugMessenger))
#endif // _DEBUG

    unsigned deviceCount = 0;
    vkEnumeratePhysicalDevices(i::GetState()->vulkan.vkInstance, &deviceCount, nullptr);

    if (!deviceCount) FRAMEWORK_ERR("There is no GPU that supports Vulkan installed in this machine.")

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(i::GetState()->vulkan.vkInstance, &deviceCount, devices.data());

    // choose first device, don't care about anything else
    i::GetState()->vulkan.physicalDevice = v::ChooseBestPhysicalDevice(devices);
    if (i::GetState()->vulkan.physicalDevice == nullptr)
        FRAMEWORK_ERR("There is no GPU installed in this machine that matches the requirements.")
}

void v::UnInitializeVulkan()
{
#ifdef _DEBUG
    v::p::DestroyDebugUtilsMessengerExt(i::GetState()->vulkan.vkInstance, i::GetState()->vulkan.debugMessenger, nullptr);
#endif // _DEBUG

    vkDestroyInstance(i::GetState()->vulkan.vkInstance, nullptr);
}

void v::CreateVulkanError(int line, int code, const char* func)
{
    std::ostringstream str;

    str << "A Vulkan API operation resulted in a fatal error:\n\n";
    str << "Error: " << code << "\n";
    str << "Origin: " << func << " at " << line << "\n\n";
    str << "This is an internal error likely caused by the framework itself. ";
    str << "The program is unable to recover and must quit now!";
    str << std::flush;

    MessageBoxA(nullptr, str.str().c_str(), "Internal Error", MB_TASKMODAL | MB_OK | MB_ICONERROR);

    i::DeAlloc();
    std::exit(-1);
}

VkPhysicalDevice v::ChooseBestPhysicalDevice(const std::vector<VkPhysicalDevice>& dev)
{
    struct GpuScore
    {
        VkPhysicalDevice dev;
        unsigned score;
    } __attribute__((aligned(16)));
    std::vector<GpuScore> rankedDevices;

    for (const VkPhysicalDevice& devi : dev)
    {
        uint32_t score = 0, queueFamCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(devi, &queueFamCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamCount);
        vkGetPhysicalDeviceQueueFamilyProperties(devi, &queueFamCount, queueFamilies.data());

        for (const VkQueueFamilyProperties& fam : queueFamilies)
        {
            if (!(fam.queueFlags & VK_QUEUE_GRAPHICS_BIT))
                break;
        }

        VkPhysicalDeviceProperties devProp{};
        vkGetPhysicalDeviceProperties(devi, &devProp);

        VkPhysicalDeviceFeatures devFeat{};
        vkGetPhysicalDeviceFeatures(devi, &devFeat);

        if (devProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            score += 300;
        if (devProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
            score += 200;
        if (devProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
            score += 100;
        if (devFeat.geometryShader)
            score += 4;
        if (devFeat.tessellationShader)
            score += 4;
        if (devFeat.multiViewport)
            score += 1;

        GpuScore gpuScore = {devi, score};
        rankedDevices.push_back(gpuScore);
    }

    GpuScore currentHigh{};
    for (GpuScore score : rankedDevices)
    {
        if (score.score > currentHigh.score) { currentHigh.score = score.score; currentHigh.dev = score.dev; }
    }

    return currentHigh.dev;
}

#ifdef _DEBUG
VkBool32 VKAPI_CALL v::ValidationDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
    VkDebugUtilsMessageTypeFlagsEXT msgType,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData)
{
    if (msgSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        i::Log(callbackData->pMessage, i::LlValidation);
    }
    return 0U;
}

VkResult v::p::CreateDebugUtilsMessengerExt(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void v::p::DestroyDebugUtilsMessengerExt(VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}
#endif // _DEBUG
