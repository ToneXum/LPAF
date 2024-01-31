// MIT License
//
// Copyright(c) 2023 ToneXum
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Framework.hpp"
#include "Internals.hpp"

void in::DoNothing_V()
{}

bool in::DoNothing_B()
{ return true; }

#ifdef _DEBUG
void in::CreateWin32Error(int line, int c, const char* func)
{
    //int e = GetLastError();
    std::ostringstream msg;
    char* eMsg = nullptr;

    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        c,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        reinterpret_cast<LPSTR>(&eMsg),
        0,
        nullptr
    );

    if (eMsg)
    {
        msg << "A Win32 API call resulted in fatal error " << c << " at line " << line << " in " << func << ".\n\n" << eMsg;
        msg << "\n" << "This is an internal error likely caused by the framework itself, the application must quit now." << std::endl;
    }
    else
    {
        msg << "An Error occoured which even the error handler could not handle. This is usually caused by the error";
        msg << " message being to long" << "\n\n";
        msg << "I guess I fucked up..." << std::endl;
    }

    MessageBoxA(nullptr, msg.str().c_str(), "Internal Error!", MB_ICONERROR | MB_TASKMODAL | MB_OK);
    LocalFree((LPSTR)eMsg);

    DeAlloc();
    std::exit(-1); // kill that fu- okay, okay ... calm down
}
#endif // _DEBUG
#ifdef NDEBUG
void in::CreateWin32Error(int line, int c, const char* func)
{
    std::wostringstream emsg;
    emsg << "Win32 error: " << c << " at " << line << " in " << func << std::flush;
    in::Log(emsg.str().c_str(), in::LL::ERROR);

    std::ostringstream msg;
    msg << "A fatal error occoured, the application must quit now!\n\nFor more information check 'Last_Log.txt' in the application";
    msg << " directory" << std::flush; 

    MessageBoxA(nullptr, msg.str().c_str(), "Fatal Error!", MB_TASKMODAL | MB_OK | MB_ICONERROR);

    DeAlloc();
    std::exit(-1);
}
#endif // DEBUG

void in::CreateManualError(int line, const char* func, const char* msg)
{
    std::ostringstream str;

    str << "An oparation within the framework has caused an error:\n\n";
    str << msg << "\n\n";
    str << "Origin: " << func << " at " << line << "\n\n";
    str << "This is an internal error likely caused by the framework itself. ";
    str << "The program is unable to recover, the application must quit now!";
    str << std::flush;

    MessageBoxA(nullptr, str.str().c_str(), "Internal Error", MB_TASKMODAL | MB_OK | MB_ICONERROR);

    DeAlloc();
    std::exit(-1);
}

void in::CreateVulkanError(int line, int c, const char* func)
{
    std::ostringstream str;

    str << "A Vulkan API oparation resulted in a fatal error:\n\n";
    str << "Error: " << c << "\n";
    str << "Origin: " << func << " at " << line << "\n\n";
    str << "This is an internal error likely caused by the framework itself. ";
    str << "The program is unable to recover and must quit now!";
    str << std::flush;

    MessageBoxA(nullptr, str.str().c_str(), "Internal Error", MB_TASKMODAL | MB_OK | MB_ICONERROR);

    DeAlloc();
    std::exit(-1);
}

void in::WindowData::MessageHandler()
{
    WIN32_EC_RET(hWnd, CreateWindowExW(
        0,
        in::AppInfo.windowClassName,
        name,
        WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
        !xPos ? CW_USEDEFAULT : xPos, !yPos ? CW_DEFAULT : yPos, // man do I love tenary expression :)
        width, height,
        nullptr,
        nullptr,
        in::AppInfo.hInstance,
        nullptr
    ));
    in::AppInfo.windowCount += 1;
    in::AppInfo.windowsOpened += 1;

    id = in::AppInfo.windowsOpened; 
    
    ShowWindow(hWnd, 1);
    
    // work is done, let the main thread continue
    std::unique_lock<std::mutex> lock(in::AppInfo.windowCreationMtx);
    AppInfo.windowCreationIsFinished = true;
    lock.unlock();
    in::AppInfo.windowCreationCv.notify_one();

    std::wostringstream msg2;
    msg2 << L"A message handler was started for " << hWnd << " for handle " << id;
    in::Log(msg2.str().c_str(), in::LL::DEBUG);
    
    // message pump for the window
    MSG msg = { };
    while (GetMessage(&msg, hWnd, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    // Window is closed, update window information
    isValid = false;

    std::wostringstream oss;
    oss << L"Message handler for " << hWnd << " for handle" << id << " has stopped";
    in::Log(oss.str().c_str(), in::LL::DEBUG);

    for (HWND i : dependers)
    {
        SendMessageA(i, WM_CLOSE, 0, 0); // destroy all dependers
    }

    EraseUnusedWindowData();

    if (!AppInfo.windowCount)
    {
        std::unique_lock<std::mutex> lock(AppInfo.threadsDoneMtx);
        AppInfo.threadsDone = true;
        lock.unlock();
        AppInfo.threadsDoneCv.notify_one();
        in::Log(L"Thread mutex was unlocked", in::LL::DEBUG);
    }
}

LRESULT in::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    ///////////////////////////////////
    // Closing the window
    case WM_CLOSE: // closing of a window has been requested
    {
        if (GetWindowData(hWnd)->OnCloseAttempt())
            DestroyWindow(hWnd);
        return 0;
    }
    case WM_DESTROY: // closing a window was ordered and confirmed
    {
        GetWindowData(hWnd)->OnClose();
        if (tsd::GetWindowCount() == 1) // quit program if last window remaining is closed
        {
            AppInfo.isRunning = false;
        }
        
        AppInfo.windowCount -= 1;
        return -1;
    }
    // Closing the window
    ///////////////////////////////////

    ///////////////////////////////////
    // Keboard and text input
    case WM_KEYDOWN:
    {
        if (!(lParam & 0x40000000)) // bitmask, check previous keystate
        {
            in::AppInfo.keystates.set(wParam);
        }
        break;
    }
    case WM_KEYUP:
    {
        in::AppInfo.keystates.reset(wParam);
        break;
    }
    case WM_SYSKEYDOWN:
    {
        if (!(lParam & 0x40000000)) // bitmask, check previous keystate
        {
            in::AppInfo.keystates.set(wParam);
        }
        break;
    }
    case WM_SYSKEYUP:
    {
        in::AppInfo.keystates.reset(wParam);
        break;
    }
    case WM_CHAR:
    { 
        if (in::AppInfo.textInputEnabled)
        {
            if (wParam != 0x0008) // is not backspace
            {
                in::AppInfo.textInput[in::AppInfo.textInputIndex] = (wchar_t)wParam;
                in::AppInfo.textInputIndex++;
            }
            else
            {
                if (in::AppInfo.textInputIndex > 0)
                    in::AppInfo.textInputIndex--;
                in::AppInfo.textInput[in::AppInfo.textInputIndex] = 0;
            }
        }
        break;
    }
    // Keboard and text input
    ///////////////////////////////////

    ///////////////////////////////////
    // Mouse input
    case WM_MOUSEMOVE:
    {
        in::AppInfo.mouse.xPos = GET_X_LPARAM(lParam);
        in::AppInfo.mouse.yPos = GET_Y_LPARAM(lParam);
        GetWindowData(hWnd)->hasMouseInClientArea = true;

        TRACKMOUSEEVENT msEv = {};
        msEv.cbSize     = sizeof(TRACKMOUSEEVENT);
        msEv.dwFlags    = TME_LEAVE;
        msEv.hwndTrack  = hWnd;

        WIN32_EC(TrackMouseEvent(&msEv));

        break;
    }
    case WM_LBUTTONDOWN:
    {
        SetCapture(hWnd);
        in::AppInfo.mouse.leftButton = true;
        break;
    }
    case WM_LBUTTONUP:
    {
        ReleaseCapture();
        in::AppInfo.mouse.leftButton = false;
        break;
    }
    case WM_MBUTTONDOWN:
    {
        SetCapture(hWnd);
        in::AppInfo.mouse.middleButton = true;
        break;
    }
    case WM_MBUTTONUP:
    {
        in::AppInfo.mouse.middleButton = false;
        break;
    }
    case WM_RBUTTONDOWN:
    {
        in::AppInfo.mouse.rightButton = true;
        break;
    }
    case WM_RBUTTONUP:
    {
        in::AppInfo.mouse.rightButton = false;
        break;
    }
    case WM_XBUTTONDOWN:
    {
        int button = HIWORD(wParam);
        switch (button)
        {
        case 1:
        {
            in::AppInfo.mouse.x1Button = true;
            break;
        }
        case 2:
        {
            in::AppInfo.mouse.x2Button = true;
            break;
        }
        }
        break;
    }
    case WM_XBUTTONUP:
    {
        int button = HIWORD(wParam);
        switch (button)
        {
        case 1:
        {
            in::AppInfo.mouse.x1Button = false;
            break;
        }
        case 2:
        {
            in::AppInfo.mouse.x2Button = false;
            break;
        }
        }
        break;
    }
    case WM_MOUSEWHEEL:
    {
        AppInfo.mouse.wheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
        break;
    }
    case WM_MOUSELEAVE:
    {
        GetWindowData(hWnd)->hasMouseInClientArea = false;
        break;
    }
    // Mouse input
    ///////////////////////////////////

    ///////////////////////////////////
    // Focus gain and loss
    case WM_SETFOCUS:
    {
        if (AppInfo.windows.empty())
            break;
        WindowData* wndData = in::GetWindowData(hWnd);
        if (wndData)
        {
            wndData->hasFocus = true;
        }

        break;
    }
    case WM_KILLFOCUS:
    {
        if (AppInfo.windows.empty())
            break;
        WindowData* wndData = in::GetWindowData(hWnd);
        if (wndData)
        {
            wndData->hasFocus = false;
        }
        in::AppInfo.keystates.reset();
        break;
    }
    // Focus gain and loss
    ///////////////////////////////////
    }
    
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

in::WindowData* in::GetWindowData(HWND handle)
{
    for (WindowData* i : AppInfo.windows)
    {
        if (i->hWnd == handle)
        {
            return i;
        }
    }
    return nullptr;
}

in::WindowData* in::GetWindowData(short id)
{
    for (WindowData* i : AppInfo.windows)
    {
        if (i->id == id)
        {
            return i;
        }
    }
    return nullptr;
}

void in::EraseUnusedWindowData()
{
    int i = 0;
    while (i < AppInfo.windows.size())
    {
        WindowData* wndDt = AppInfo.windows.at(i);
        if (!wndDt->isValid)
        {
            std::wostringstream oss;
            oss << L"Window data for " << wndDt->hWnd << " with the handle " << wndDt->id << " was deleted" << std::flush;
            in::Log(oss.str().c_str(), in::LL::DEBUG);

            delete wndDt->msgThread;
            delete wndDt;
            std::vector<WindowData*>::iterator it = AppInfo.windows.begin() + i;
            AppInfo.windows.erase(it);
        }
        else
        {
            i++;
        }
    }
}

void in::DeAlloc()
{
    for (WindowData* i : AppInfo.windows)
    {
        delete i->msgThread;
        delete i;
    }
    delete AppInfo.textInput;
}

void in::InitialiseVulkan()
{
    // do setup for validation layers
#ifdef _DEBUG
    // make sure that all layers requested are present and enabled
    unsigned int layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    bool allLayersFound = true;
    for (const char* layerName : RenderInfo.validationLayers) // loops through the layers we want
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
    mssngrInf.pfnUserCallback = in::ValidationDegubCallback;
    mssngrInf.pUserData = nullptr;
#endif // _DEBUG

    VkApplicationInfo appInf{};
    appInf.sType            = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInf.pApplicationName = "GAFW Renderer";
    appInf.apiVersion       = VK_MAKE_API_VERSION(1, 1, 0, 0);
    appInf.pEngineName      = "No Engine";
    appInf.engineVersion    = VK_MAKE_API_VERSION(1, 1, 0, 0);
    appInf.apiVersion       = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInf{};
    createInf.sType                 = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInf.pApplicationInfo      = &appInf;
#ifdef _DEBUG
    createInf.enabledLayerCount     = (unsigned int)in::RenderInfo.validationLayers.size();
    createInf.ppEnabledLayerNames   = in::RenderInfo.validationLayers.data();
    createInf.pNext                 = &mssngrInf;
#endif // _DEBUG
    createInf.enabledExtensionCount = (unsigned int)in::RenderInfo.extension.size();
    createInf.ppEnabledExtensionNames = in::RenderInfo.extension.data();

    VUL_EC(vkCreateInstance(&createInf, nullptr, &RenderInfo.vkInstance));
    in::Log(L"A Vulkan instance was created", in::LL::INFO);

#ifdef _DEBUG
    VUL_EC(prx::vkCreateDebugUtilsMessengerEXT(in::RenderInfo.vkInstance, &mssngrInf, nullptr, &in::RenderInfo.debugMessenger));
#endif // _DEBUG

    unsigned deviceCount = 0;
    vkEnumeratePhysicalDevices(in::RenderInfo.vkInstance, &deviceCount, nullptr);

    if (!deviceCount) FRMWRK_ERR("There is no GPU that supports Vulkan installed in this machine.");
    
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(in::RenderInfo.vkInstance, &deviceCount, devices.data());

    in::RenderInfo.physicalDevice = in::ChooseBestPhysicalDevice(devices); // choose first device, dont care about anything else
    if (in::RenderInfo.physicalDevice == nullptr) 
        FRMWRK_ERR("There is no GPU installed in this machine that matches the requirements.");
}

void in::UninitialiseVulkan()
{
#ifdef _DEBUG
    prx::vkDestroyDebugUtilsMessengerEXT(in::RenderInfo.vkInstance, in::RenderInfo.debugMessenger, nullptr);
#endif // _DEBUG

    vkDestroyInstance(in::RenderInfo.vkInstance, nullptr);
}

VkPhysicalDevice in::ChooseBestPhysicalDevice(const std::vector<VkPhysicalDevice>& dev)
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
VkBool32 VKAPI_CALL in::ValidationDegubCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity, 
    VkDebugUtilsMessageTypeFlagsEXT msgType, 
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData, 
    void* userData)
{
    if (msgSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        in::Log(callbackData->pMessage, in::LL::VALIDATION);
    }
    return 0u;
}

VkResult prx::vkCreateDebugUtilsMessengerEXT(VkInstance instance,
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

void prx::vkDestroyDebugUtilsMessengerEXT(VkInstance instance,
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
void tsd::Initialise(int iconId, int cursorId)
{
    // Get hInstance since the program does not use the winMain entry point
    in::AppInfo.hInstance = GetModuleHandle(0);

    // Check the recourses, if invalid continue anyways
    if (iconId)
    {
        in::AppInfo.hIcon = (HICON)LoadImageA(in::AppInfo.hInstance, MAKEINTRESOURCE(iconId), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
        if (!in::AppInfo.hIcon) 
        { 
            in::Log(L"Specified recourse Id for an icon was invalid", in::LL::ERROR);
        }
    }

    if (cursorId)
    {
        in::AppInfo.hCursor = (HCURSOR)LoadImageA(in::AppInfo.hInstance, MAKEINTRESOURCE(cursorId), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
        if (!in::AppInfo.hCursor) 
        { 
            in::Log(L"Specified recourse Id for a mouse was invalid", in::LL::ERROR);
        }
    }
    
    WNDCLASSEXW wc = {};

    wc.cbClsExtra       = 0;
    wc.cbSize           = sizeof(WNDCLASSEXW);
    wc.cbWndExtra       = 0;
    wc.hbrBackground    = nullptr;
    wc.hCursor          = in::AppInfo.hCursor;
    wc.hIcon            = in::AppInfo.hIcon;
    wc.hIconSm          = in::AppInfo.hIcon;
    wc.hInstance        = in::AppInfo.hInstance;
    wc.lpfnWndProc      = in::WindowProc;
    wc.lpszClassName    = in::AppInfo.windowClassName;
    wc.lpszMenuName     = nullptr;
    wc.style            = 0;

    WIN32_EC_RET(in::AppInfo.classAtom, RegisterClassExW(&wc));

    // allocate memory for text input field
    in::AppInfo.textInput = new wchar_t[100000]{0}; // thats 200 whole kilobytes of ram right there

    in::AppInfo.isInitialised = true;

    in::InitialiseVulkan();
    in::Log(L"Framework was successfully initialised", in::LL::INFO);
}

void tsd::Uninitialise()
{
    std::unique_lock<std::mutex> lock(in::AppInfo.threadsDoneMtx);
    in::AppInfo.threadsDoneCv.wait(lock, []{ return in::AppInfo.threadsDone; }); // wait for the last thread to end

    // not neccecary to delete the window data, it was deleted by EraseUnusedWindowData()
    delete in::AppInfo.textInput;

    WIN32_EC(UnregisterClassW(in::AppInfo.windowClassName, in::AppInfo.hInstance));

    in::UninitialiseVulkan();

    in::Log(L"Framework was successfully uninitialised", in::LL::INFO);
}

void in::Log(const wchar_t* msg, LL ll)
{
#ifdef _DEBUG
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t currentDate = std::chrono::system_clock::to_time_t(now);

    char timeBuf[30] = { 0 }; // minimum required size for this is 26. Who knows if this is going to run in 9997976 years?
    ctime_s(timeBuf, sizeof(timeBuf), &currentDate);
    *std::strchr(timeBuf, '\n') = 0; // replace that pesky newline with the null-char

    // extra buffer, prevents asychrony from messing with the output when this func is called from different threads at the same time
    std::wostringstream oss; 
    oss << "[ " << timeBuf << " ]";

    switch (ll)
    {
    case in::LL::INFO:
    {
        oss << " [ INFO ]: ";
        break;
    }
    case in::LL::DEBUG:
    {
        oss << " [ DEBUG ]: ";
        break;
    }
    case in::LL::WARNING:
    {
        oss << " [ WARNING ]: ";
        break;
    }
    case in::LL::ERROR:
    {
        oss << " [ ERROR ]: ";
        break;
    }
    case in::LL::VALIDATION:
    {
        oss << " [ VALID ]: ";
    }
    }

    AppInfo.logMtx.lock();
    std::wcout << oss.str().c_str() << msg << "\n" << std::flush;
    AppInfo.logMtx.unlock();
#endif // _DEBUG
}

void in::Log(const char* msg, LL ll)
{
#ifdef _DEBUG
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t currentDate = std::chrono::system_clock::to_time_t(now);

    char timeBuf[30] = { 0 }; // minimum required size for this is 26. Who knows if this is going to run in 9997976 years?
    ctime_s(timeBuf, sizeof(timeBuf), &currentDate);
    *std::strchr(timeBuf, '\n') = 0; // replace that pesky newline with the null-char

    // extra buffer, prevents asychrony from messing with the output when this func is called from different threads at the same time
    std::ostringstream oss;
    oss << "[ " << timeBuf << " ]";

    switch (ll)
    {
    case in::LL::INFO:
    {
        oss << " [ INFO ]: ";
        break;
    }
    case in::LL::DEBUG:
    {
        oss << " [ DEBUG ]: ";
        break;
    }
    case in::LL::WARNING:
    {
        oss << " [ WARNING ]: ";
        break;
    }
    case in::LL::ERROR:
    {
        oss << " [ ERROR ]: ";
        break;
    }
    case in::LL::VALIDATION:
    {
        oss << " [ VALID ]: ";
    }
    }

    AppInfo.logMtx.lock();
    std::cout << oss.str().c_str() << msg << "\n" << std::flush;
    AppInfo.logMtx.unlock();
#endif // _DEBUG
}

// Whoops
#undef CreateWindow
short tsd::CreateWindow(const wchar_t* name, int width, int height, int xPos, int yPos, const short* dependants, unsigned depCount)
{
    if (!in::AppInfo.isInitialised) { in::Log(L"CreateWindow() was called before initialisation", in::LL::ERROR); return 0; } // init was not called
    if (!name) { in::Log(L"Nullptr was passed to a required parameter at CreateWindow()", in::LL::ERROR); return 0; } // name is nullptr
    if ((height <= 0) || (width <= 0)) { in::Log(L"Invalid heigt or width ammount for window creation", in::LL::ERROR); return 0; }

    in::WindowData* wndData = new in::WindowData;
    wndData->msgThread = new std::thread(&in::WindowData::MessageHandler, wndData);
    wndData->msgThread->detach();

    wndData->name       = const_cast<wchar_t*>(name);
    wndData->width      = width;
    wndData->height     = height;
    wndData->xPos       = xPos;
    wndData->yPos       = yPos;
    wndData->isValid    = true;
    wndData->hasFocus   = true;
    wndData->hasMouseInClientArea = false;

    // wait for window creation to finish
    std::unique_lock<std::mutex> lock(in::AppInfo.windowCreationMtx);
    in::AppInfo.windowCreationCv.wait(lock, [] { return in::AppInfo.windowCreationIsFinished; });
    
    if (dependants && depCount)
    {
        for (unsigned i = 0; i < depCount; i++)
        {
            // this is unsafe like shit
            wndData->dependers.push_back(in::GetWindowData(dependants[i])->hWnd);
        }
    }

    in::AppInfo.windows.push_back(wndData);

    in::AppInfo.windowCreationIsFinished = false;

    // ran out of range
    if (wndData->id == SHRT_MAX)
    {
        in::Log(L"Handle out of range, window was opened but no handle could be created. You now have a stray window on you desktop", in::LL::ERROR);
        return 0;
    }

    std::wostringstream oss;
    oss << L"Window " << wndData->hWnd << " was created and recieved a handle of " << wndData->id;
    in::Log(oss.str().c_str(), in::LL::DEBUG);
    return wndData->id;
}

void tsd::OnWindowCloseAttempt(short handle, bool(*func)(void))
{
    in::GetWindowData(handle)->OnCloseAttempt = func;
}

void tsd::OnWindowClose(short handle, void(*func)(void))
{
    in::GetWindowData(handle)->OnClose = func;
}

wchar_t* tsd::GetWindowName(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::Log(L"Invalid handle was passed to GetWindowName()", in::LL::WARNING); return nullptr; }
    return wndData->name;
}

bool tsd::GetWindowVisibility(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) [[unlikely]] { in::Log(L"Invalid handle was passed to GetWindowVisibility()", in::LL::WARNING); return false; }
    return wndData->isVisible;
}

int tsd::GetWindowWidth(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) [[unlikely]] { in::Log(L"Invalid handle was passed to GetWindowWidth()", in::LL::WARNING); return false; }
    return wndData->width;
}

int tsd::GetWindowHeight(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) [[unlikely]] { in::Log(L"Invalid handle was passed to GetWindowHeight()", in::LL::WARNING); return false; }
    return wndData->height;
}

std::pair<int, int> tsd::GetWindowDimensions(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) [[unlikely]] { in::Log(L"Invalid handle was passed to GetWindowDimensions()", in::LL::WARNING); return {0, 0}; }
    return {wndData->width, wndData->height};
}

int tsd::GetWindowXPos(short id, WP wpr)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) [[unlikely]] { in::Log(L"Invalid handle was passed to GetWindowXPos()", in::LL::WARNING); return 0; }
    RECT rect{};
    GetWindowRect(wndData->hWnd, &rect);

    switch (wpr)
    {
    case WP_LEFT:
        return rect.left;
    case WP_RIGHT:
        return rect.right;
    }

    in::Log(L"Invalid positional identifier was passed to GetWindowXPos()", in::LL::WARNING);
    return 0;
}

int tsd::GetWindowYPos(short id, WP wpr)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) [[unlikely]] { in::Log(L"Invalid handle was passed to GetWindowYPos()", in::LL::WARNING); return 0; }
    RECT rect{};
    GetWindowRect(wndData->hWnd, &rect);

    switch (wpr)
    {
    case WP_TOP:
        return rect.top;
    case WP_BOTTOM:
        return rect.bottom;
    }

    in::Log(L"Invalid positional identifier was passed to GetWindowYPos()", in::LL::WARNING);
    return 0;
}

std::pair<int, int> tsd::GetWindowPosition(short id, WP wpr)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) [[unlikely]] { in::Log(L"Invalid handle was passed to GetWindowPosition()", in::LL::WARNING); return {0, 0}; }
    RECT rect{};
    GetWindowRect(wndData->hWnd, &rect);
    
    switch (wpr)
    {
    case WP_TOP_LEFT:
        return { rect.left, rect.top };
    case WP_TOP_RIGHT:
        return { rect.right, rect.top };
    case WP_BOTTOM_LEFT:
        return { rect.left, rect.bottom };
    case WP_BOTTOM_RIGHT:
        return { rect.right, rect.bottom };
    }

    in::Log(L"Invalid positional identifier was passed to GetWindowPosition()", in::LL::WARNING);
    return {0, 0};
}

int tsd::GetWindowCount(void)
{
    return in::AppInfo.windowCount;
}

bool tsd::ChangeWindowName(short id, const wchar_t* name)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) [[unlikely]] { in::Log(L"Invalid handle was passed to ChangeWindowName()", in::LL::ERROR); return false; }
    SetWindowTextW(wndData->hWnd, name);
    wndData->name = (wchar_t*)name;
    return true;
}

bool tsd::WindowHasFocus(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) [[unlikely]] { return false; }
    return wndData->hasFocus;
}

bool tsd::IsValidHandle(short handle)
{
    if (in::GetWindowData(handle))
        return true;
    return false;
}

bool tsd::Running()
{
    return in::AppInfo.isRunning;
}

void tsd::Halt(int ms)
{
    Sleep(ms);
}

#undef MessageBox
#undef IGNORE
int tsd::MessageBox(short owner, const wchar_t* title, const wchar_t* msg, int flags)
// by now win32 is just getting anoying
#ifdef UNICODE
#define MessageBox  MessageBoxW
#else
#define MessageBox  MessageBoxA
#endif
{
    // return null if the window is not found so I dont care
    in::WindowData* ownerData = in::GetWindowData(owner);

    long rawFlags = 0;

    // Where switch statement?
    // Cant put (non-constant) expressions into switch cases
    if (flags & MF_TASKMODAL)
        rawFlags = rawFlags | MB_TASKMODAL;

    if (flags & MF_ICON_WARNING)
        rawFlags = rawFlags | MB_ICONWARNING;
    if (flags & MF_ICON_ERROR)
        rawFlags = rawFlags | MB_ICONERROR;
    if (flags & MF_ICON_INFO)
        rawFlags = rawFlags | MB_ICONINFORMATION;
    if (flags & MF_ICON_QUESTION)
        rawFlags = rawFlags | MB_ICONQUESTION;
    
    if (flags & MF_BUTTON_OK)
        rawFlags = rawFlags | MB_OK;
    if (flags & MF_BUTTON_OK_CANCEL)
        rawFlags = rawFlags | MB_OKCANCEL;
    if (flags & MF_BUTTON_YES_NO)
        rawFlags = rawFlags | MB_YESNO;
    if (flags & MF_BUTTON_RETRY_CANEL)
        rawFlags = rawFlags | MB_RETRYCANCEL;

    if (flags & MF_BUTTON_YES_NO_CANCEL)
        rawFlags = rawFlags | MB_YESNOCANCEL;
    if (flags & MF_BUTTON_ABORT_RETRY_IGNORE)
        rawFlags = rawFlags | MB_ABORTRETRYIGNORE;
    if (flags & MF_BUTTON_CANCEL_RETRY_CONTINUE)
        rawFlags = rawFlags | MB_CANCELTRYCONTINUE;

    int result = MessageBoxW(ownerData ? ownerData->hWnd : 0, msg, title, rawFlags);
    if (result == 0)
        in::Log(L"Invalid set of flags where passed to MessageBox()", in::LL::ERROR);

    switch (result)
    {
    case IDABORT:       return MF_ABORT;
    case IDCANCEL:      return MF_CANCEL;
    case IDCONTINUE:    return MF_CONTINUE;
    case IDIGNORE:      return MF_IGNORE;
    case IDNO:          return MF_NO;
    case IDOK:          return MF_OK;
    case IDRETRY:       return MF_RETRY;
    case IDTRYAGAIN:    return MF_TRYAGAIN;
    case IDYES:         return MF_YES;
    }
    return MF_CANCEL; // should never reach this
}
#define IGNORE 0

bool tsd::IsKeyPressed(Key code)
{
    return in::AppInfo.keystates.test((int)code);
}

bool tsd::IsKeyPressedOnce(Key code)
{
    bool state = in::AppInfo.keystates.test((int)code);
    if (state)
        in::AppInfo.keystates.reset((int)code);
    return state;
}

bool tsd::IsKeyReleased(Key code)
{
    static bool lastState;
    if (lastState && !IsKeyPressed(code))
    {
        lastState = IsKeyPressed(code);
        return true;
    }
    lastState = IsKeyPressed(code);
    return false;
}

bool tsd::IsAnyKeyPressed()
{
    return in::AppInfo.keystates.any();
}

void tsd::SetTextInputState(bool state, bool clear)
{
    in::AppInfo.textInputEnabled = state;
    if (clear)
    {
        for (int i = 0; i < 100000; i++)
        {
            in::AppInfo.textInput[i] = 0;
        }
        in::AppInfo.textInputIndex = 0;
    }
}

wchar_t* tsd::GetTextInput()
{
    return in::AppInfo.textInput;
}

void tsd::ClearTextInput()
{
    for (int i = 0; i < 100000; i++)
    {
        in::AppInfo.textInput[i] = 0;
    }
    in::AppInfo.textInputIndex = 0;
}

bool tsd::IsTextInputEnabled()
{
    return in::AppInfo.textInputEnabled;
}

tsd::MouseInfo tsd::GetMouseInfo()
{
    MouseInfo msInfo = {};

    msInfo.left     = in::AppInfo.mouse.leftButton;
    msInfo.right    = in::AppInfo.mouse.rightButton;
    msInfo.middle   = in::AppInfo.mouse.middleButton;
    msInfo.x1       = in::AppInfo.mouse.x1Button;
    msInfo.x2       = in::AppInfo.mouse.x2Button;
    msInfo.xPos     = in::AppInfo.mouse.xPos;
    msInfo.yPos     = in::AppInfo.mouse.yPos;
    msInfo.containingWindow = tsd::GetMouseContainerWindow();

    return msInfo;
}

int tsd::GetMouseX()
{
    return in::AppInfo.mouse.xPos;
}

int tsd::GetMouseY()
{
    return in::AppInfo.mouse.yPos;
}

bool tsd::GetMouseLeftButton()
{
    return in::AppInfo.mouse.leftButton;
}

bool tsd::GetMouseLeftButtonOnce()
{
    bool state = in::AppInfo.mouse.leftButton;
    if (state)
        in::AppInfo.mouse.leftButton = false;
    return state;
}

bool tsd::GetMouseRightButton()
{
    return in::AppInfo.mouse.rightButton;
}

bool tsd::GetMouseRightButtonOnce()
{
    bool state = in::AppInfo.mouse.rightButton;
    if (state)
        in::AppInfo.mouse.rightButton = false;
    return state;
}

bool tsd::GetMouseMiddleButton()
{
    return in::AppInfo.mouse.middleButton;
}

bool tsd::GetMouseMiddleButtonOnce()
{
    bool state = in::AppInfo.mouse.middleButton;
    if (state)
        in::AppInfo.mouse.middleButton = false;
    return state;
}

bool tsd::GetMouseX1Button()
{
    return in::AppInfo.mouse.x1Button;
}

bool tsd::GetMouseX1ButtonOnce()
{
    bool state = in::AppInfo.mouse.x1Button;
    if (state)
        in::AppInfo.mouse.x1Button = false;
    return state;
}

bool tsd::GetMouseX2Button()
{
    return in::AppInfo.mouse.x2Button;
}

bool tsd::GetMouseX2ButtonOnce()
{
    bool state = in::AppInfo.mouse.x2Button;
    if (state)
        in::AppInfo.mouse.x2Button = false;
    return state;
}

int tsd::GetMouseWheelDelta()
{
    static int lastDelta;

    if (lastDelta < in::AppInfo.mouse.wheelDelta)
    {
        lastDelta = in::AppInfo.mouse.wheelDelta;
        return 1;
    }

    else if (lastDelta > in::AppInfo.mouse.wheelDelta)
    {
        lastDelta = in::AppInfo.mouse.wheelDelta;
        return -1;
    }

    lastDelta = in::AppInfo.mouse.wheelDelta;
    return 0;
}

bool tsd::WindowContainsMouse(short handle)
{
    in::WindowData* wndData = in::GetWindowData(handle);
    if (wndData) [[likely]]
        return wndData->hasMouseInClientArea;
    return false;
}

short tsd::GetMouseContainerWindow()
{
    for (in::WindowData* i : in::AppInfo.windows)
    {
        if (i->hasMouseInClientArea)
            return i->id;
    }
    return 0;
}
