#include "Vulkan.hpp"
#include "Internals.hpp"

// About this file:
// Here are all the implementation for Vulkan related operations

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
        for (const VkLayerProperties& layerProperties : availableLayers) // loop throught the layer we have
        {
            if (strcmp(layerName, layerProperties.layerName) == 0) // strcmp does a bitmask, 0 means strings are equal
            {
                found = true;
                break;
            }
        }
        if (!found) allLayersFound = false;
    }

    if (!allLayersFound) FRMWRK_ERR("Not all validation layers requested where found.");

    VkDebugUtilsMessengerCreateInfoEXT mssngrInf{};
    mssngrInf.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    mssngrInf.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    mssngrInf.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    mssngrInf.pfnUserCallback = v::ValidationDegubCallback;
    mssngrInf.pUserData = nullptr;
#endif // _DEBUG

    VkApplicationInfo appInf{};
    appInf.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInf.pApplicationName = "GAFW Renderer";
    appInf.apiVersion = VK_MAKE_API_VERSION(1, 1, 0, 0);
    appInf.pEngineName = "No Engine";
    appInf.engineVersion = VK_MAKE_API_VERSION(1, 1, 0, 0);
    appInf.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInf{};
    createInf.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInf.pApplicationInfo = &appInf;
#ifdef _DEBUG
    createInf.enabledLayerCount = (unsigned int)i::GetState()->vulkan.validationLayers.size();
    createInf.ppEnabledLayerNames = i::GetState()->vulkan.validationLayers.data();
    createInf.pNext = &mssngrInf;
#endif // _DEBUG
    createInf.enabledExtensionCount = (unsigned int)i::GetState()->vulkan.extension.size();
    createInf.ppEnabledExtensionNames = i::GetState()->vulkan.extension.data();

    VUL_EC(vkCreateInstance(&createInf, nullptr, &i::GetState()->vulkan.vkInstance));
    i::Log(L"A Vulkan instance was created", i::LL::INFO);

#ifdef _DEBUG
    VUL_EC(v::vkCreateDebugUtilsMessengerEXT(i::GetState()->vulkan.vkInstance, &mssngrInf, nullptr, &i::GetState()->vulkan.debugMessenger));
#endif // _DEBUG

    unsigned deviceCount = 0;
    vkEnumeratePhysicalDevices(i::GetState()->vulkan.vkInstance, &deviceCount, nullptr);

    if (!deviceCount) FRMWRK_ERR("There is no GPU that supports Vulkan installed in this machine.");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(i::GetState()->vulkan.vkInstance, &deviceCount, devices.data());

    i::GetState()->vulkan.physicalDevice = v::ChooseBestPhysicalDevice(devices); // choose first device, dont care about anything else
    if (i::GetState()->vulkan.physicalDevice == nullptr)
        FRMWRK_ERR("There is no GPU installed in this machine that matches the requirements.");
}

void v::UninitialiseVulkan()
{
#ifdef _DEBUG
    v::vkDestroyDebugUtilsMessengerEXT(i::GetState()->vulkan.vkInstance, i::GetState()->vulkan.debugMessenger, nullptr);
#endif // _DEBUG

    vkDestroyInstance(i::GetState()->vulkan.vkInstance, nullptr);
}

void v::CreateVulkanError(int line, int c, const char* func)
{
    std::ostringstream str;

    str << "A Vulkan API oparation resulted in a fatal error:\n\n";
    str << "Error: " << c << "\n";
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
    struct GPUScore
    {
        unsigned score;
        VkPhysicalDevice dev;
    };
    std::vector<GPUScore> rankedDevices;

    for (const VkPhysicalDevice& devi : dev)
    {
        unsigned score = 0, queueFamCount = 0; bool isQualified = true;
        vkGetPhysicalDeviceQueueFamilyProperties(devi, &queueFamCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamCount);
        vkGetPhysicalDeviceQueueFamilyProperties(devi, &queueFamCount, queueFamilies.data());

        for (const VkQueueFamilyProperties& fam : queueFamilies)
        {
            if (!fam.queueFlags & VK_QUEUE_GRAPHICS_BIT)
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

        GPUScore gpuScore = { score, devi };
        rankedDevices.push_back(gpuScore);
    }

    GPUScore currentHigh{};
    for (GPUScore s : rankedDevices)
    {
        if (s.score > currentHigh.score) { currentHigh.score = s.score; currentHigh.dev = s.dev; }
    }

    return currentHigh.dev;
}

#ifdef _DEBUG
VkBool32 VKAPI_CALL v::ValidationDegubCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
    VkDebugUtilsMessageTypeFlagsEXT msgType,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData)
{
    if (msgSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        i::Log(callbackData->pMessage, i::LL::VALIDATION);
    }
    return 0u;
}

VkResult v::vkCreateDebugUtilsMessengerEXT(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void v::vkDestroyDebugUtilsMessengerEXT(VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}
#endif // _DEBUG
