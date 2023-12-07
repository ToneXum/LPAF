#include "Framework.hpp"
#include "Internals.hpp"

void in::CreateWin32DebugError(int line)
{
    int e = GetLastError();
    std::ostringstream msg;
    char* eMsg = nullptr;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        e,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        reinterpret_cast<LPSTR>(&eMsg),
        0,
        nullptr
    );

    if (eMsg)
    {
        msg << "A Win32 API call resulted in a fatal error " << e << " at line " << line << " in the source of the framework.\n\n" << eMsg;
        msg << "\n" << "This is an internal error likely caused by the framework itself, the application must quit now. If this ";
        msg << "keeps happening then notify the dev about his fuckup." << std::endl;
    }
    else
    {
        msg << "An Error occoured which even the error handler could not handle. This is usually caused by the error";
        msg << " message being to long" << "\n\n";
        msg << "I guess I fucked up..." << std::endl;
    }

    MessageBox(nullptr, msg.str().c_str(), "Internal Error!", MB_ICONERROR | MB_TASKMODAL | MB_OK);
    LocalFree((LPSTR)eMsg);

    std::exception exc;
    throw exc;
}

void in::CreateWin32ReleaseError(int line)
{
    std::ofstream file("Last_Log.txt");
    
    int e = GetLastError();

    file << "[ " << __TIMESTAMP__ << " ]: " << "Unhandled Win32 error! " << e << " at " << line << "\n";
    file << "[ " << __TIMESTAMP__ << " ]: " << "Fatal Error, application must abort!" << std::endl;

    file.close();

    std::exception exc;
    throw exc;
}

void in::SetLastError(int code)
{
    WindowInfo.lastErrorCode = code;
}

void in::WindowData::MessageHandler()
{
    WIN32_EC_RET(hWnd, CreateWindowEx(
        0,
        in::WindowInfo.windowClassName,
        name,
        WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
        !xPos ? CW_USEDEFAULT : xPos, !yPos ? CW_DEFAULT : yPos, // man do I love tenary expression :)
        width, height,
        nullptr,
        nullptr,
        in::WindowInfo.hInstance,
        nullptr
    ));
    in::WindowInfo.windowCount += 1;
    in::WindowInfo.windowsOpened += 1;

    id = in::WindowInfo.windowsOpened; 
    
    ShowWindow(hWnd, 1);
    
    // work is done, let the main thread continue
    std::unique_lock<std::mutex> lock1(in::WindowInfo.mtx);
    WindowInfo.windowIsFinished = true;
    lock1.unlock();
    in::WindowInfo.cv.notify_one();

    // message pump for the window
    MSG msg = { };
    while (GetMessage(&msg, hWnd, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    WindowInfo.windowCount -= 1;
    isValid = false;
}

LRESULT in::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE: // closing of a window has been requested
    {
        DestroyWindow(hWnd);
        break;
    }
    case WM_DESTROY: // closing a window was ordered and confirmed
    {
        if (in::GetWindowData(hWnd)->id == 1) // quit program if origin window is closed
        {
            WindowInfo.isRunning = false;
            PostQuitMessage(0);
            return 0;
        }
        break;
    }
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

in::WindowData* in::GetWindowData(HWND handle)
{
    for (WindowData* i : WindowInfo.windows)
    {
        if ((i->hWnd == handle) && (i->isValid))
        {
            return i;
        }
    }
    return nullptr;
}

in::WindowData* in::GetWindowData(int id)
{
    for (WindowData* i : WindowInfo.windows)
    {
        if ((i->id == id) && (i->isValid))
        {
            return i;
        }
    }
    return nullptr;
}

void tsd::Initialise(void)
{
    // Get hInstance since the program does not use the winMain entry point
    in::WindowInfo.hInstance = GetModuleHandle(0);

    WNDCLASSEX wc = {};

    wc.cbClsExtra       = 0;
    wc.cbSize           = sizeof(WNDCLASSEX);
    wc.cbWndExtra       = 0;
    wc.hbrBackground    = nullptr;
    wc.hCursor          = nullptr;
    wc.hIcon            = nullptr;
    wc.hIconSm          = nullptr;
    wc.hInstance        = in::WindowInfo.hInstance;
    wc.lpfnWndProc      = in::WindowProc;
    wc.lpszClassName    = in::WindowInfo.windowClassName;
    wc.lpszMenuName     = nullptr;
    wc.style            = 0;

    WIN32_EC_RET(in::WindowInfo.classAtom, RegisterClassEx(&wc));

    in::WindowInfo.isInitialised = true;
}

void tsd::Uninitialise(void)
{
    for (in::WindowData* w : in::WindowInfo.windows)
        w->msgThread->join();

    WIN32_EC(UnregisterClass(in::WindowInfo.windowClassName, in::WindowInfo.hInstance));

    for (in::WindowData* w : in::WindowInfo.windows)
    {
        delete w->msgThread;
        delete w;
    }
}

// Whoops
#undef CreateWindow

short tsd::CreateWindow(const char* name, int width, int height, int xPos, int yPos)
{
    if (!in::WindowInfo.isInitialised) { in::SetLastError(2); return 0; }
    if (!name) { in::SetLastError(3); return 0; }
    if ((height <= 0) || (width <= 0)) { in::SetLastError(3); return 0; }

    in::WindowData* wndData = new in::WindowData;
    wndData->msgThread = new std::thread(&in::WindowData::MessageHandler, wndData);

    wndData->name       = const_cast<char*>(name);
    wndData->width      = width;
    wndData->height     = height;
    wndData->xPos       = xPos;
    wndData->yPos       = yPos;
    wndData->isValid    = true;

    // wait for window creation to finish
    std::unique_lock<std::mutex> lock(in::WindowInfo.mtx);
    in::WindowInfo.cv.wait(lock, [] { return in::WindowInfo.windowIsFinished; });

    in::WindowInfo.windows.push_back(wndData);

    in::WindowInfo.windowIsFinished = false;

    return wndData->id;
}

int tsd::GetLastError()
{
    return in::WindowInfo.lastErrorCode;
}

const char* tsd::GetErrorInformation(int code)
{
    return in::errors.find(code) != in::errors.end() ? in::errors[code] : "Invalid error Code!"; // best one-liner so far
}

char* tsd::GetWindowName(short id)
{
    return in::GetWindowData(id)->name;
}

bool tsd::GetWindowVisibility(short id)
{
    return in::GetWindowData(id)->isVisible;
}

int tsd::GetWindowWidth(short id)
{
    return in::GetWindowData(id)->width;
}

int tsd::GetWindowHeight(short id)
{
    return in::GetWindowData(id)->height;
}

int tsd::GetWindowCount(void)
{
    return in::WindowInfo.windowCount;
}

bool tsd::Running()
{
    return in::WindowInfo.isRunning;
}

void tsd::Halt(int ms)
{
    Sleep(ms);
}

#undef IGNORE
tsd::MBR tsd::CreateMessageBox(short owner, const char* title, const char* msg, int flags)
{
    // return null if the window is not found so I dont care
    in::WindowData* ownerData = in::GetWindowData(owner);

    long rawFlags = 0;

    // Where switch statement?
    // Cant put expressions into switch cases
    if (flags & tsd::MBF::TASKMODAL)
        rawFlags = rawFlags | MB_TASKMODAL;

    if (flags & tsd::MBF::ICON_WARNING)
        rawFlags = rawFlags | MB_ICONWARNING;
    if (flags & tsd::MBF::ICON_ERROR)
        rawFlags = rawFlags | MB_ICONERROR;
    if (flags & tsd::MBF::ICON_INFO)
        rawFlags = rawFlags | MB_ICONINFORMATION;
    if (flags & tsd::MBF::ICON_QUESTION)
        rawFlags = rawFlags | MB_ICONQUESTION;

    if (flags & tsd::MBF::BUTTON_OK)
        rawFlags = rawFlags | MB_OK;
    if (flags & tsd::MBF::BUTTON_OK_CANCEL)
        rawFlags = rawFlags | MB_OKCANCEL;
    if (flags & tsd::MBF::BUTTON_YES_NO)
        rawFlags = rawFlags | MB_YESNO;
    if (flags & tsd::MBF::BUTTON_RETRY_CANEL)
        rawFlags = rawFlags | MB_RETRYCANCEL;

    if (flags & tsd::MBF::BUTTON_YES_NO_CANCEL)
        rawFlags = rawFlags | MB_YESNOCANCEL;
    if (flags & tsd::MBF::BUTTON_ABORT_RETRY_IGNORE)
        rawFlags = rawFlags | MB_ABORTRETRYIGNORE;
    if (flags & tsd::MBF::BUTTON_CANCEL_RETRY_CONTINUE)
        rawFlags = rawFlags | MB_CANCELTRYCONTINUE;

    // intentional nullptr reference
    int result = MessageBox(ownerData ? ownerData->hWnd : 0, msg, title, rawFlags);


    switch (result)
    {
    case IDABORT: return tsd::MBR::ABORT;
    case IDCANCEL: return tsd::MBR::CANCEL;
    case IDCONTINUE: return tsd::MBR::CONTINUE;
    case IDIGNORE: return tsd::MBR::IGNORE;
    case IDNO: return tsd::MBR::NO;
    case IDOK: return tsd::MBR::OK;
    case IDRETRY: return tsd::MBR::RETRY;
    case IDTRYAGAIN: return tsd::MBR::TRYAGAIN;
    case IDYES: return tsd::MBR::YES;
    }
}
#define IGNORE 0
