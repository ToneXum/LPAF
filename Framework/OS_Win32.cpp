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

#include "Internals.hpp"

#ifdef _WINDOWS
#ifdef _DEBUG
void i::CreateWin32Error(int line, int code, const char* func)
{
    //int e = GetLastError();
    std::ostringstream msg;
    char* errorMessage = nullptr;

    FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            code,
            MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
            static_cast<LPSTR>(errorMessage),
            0,
            nullptr
    );

    msg << "A Win32 API call resulted in fatal error " << code << " at line " << line << " in " << func << ".\n\n";
    msg << errorMessage << "\n";
    msg << "This is an internal error likely caused by the framework itself, the application must quit now.\n";

    MessageBoxA(nullptr, msg.str().c_str(), "Internal Error!", MB_ICONERROR | MB_TASKMODAL | MB_OK);
    LocalFree(static_cast<LPSTR>(errorMessage));

    DeAlloc();
    std::exit(-1);
}
#endif // _DEBUG

#ifdef NDEBUG
void i::CreateWin32Error(int line, int c, const char* func)
{
    std::wostringstream emsg;
    emsg << "Win32 error: " << c << " at " << line << " in " << func << std::flush;
    i::Log(emsg.str().c_str(), i::LlError);

    std::ostringstream msg;
    msg << "A fatal error occoured, the application must quit now!\n\nFor more information check 'Last_Log.txt' in the application";
    msg << " directory" << std::flush;

    MessageBoxA(nullptr, msg.str().c_str(), "Fatal Error!", MB_TASKMODAL | MB_OK | MB_ICONERROR);

    DeAlloc();
    std::exit(-1);
}
#endif // NDEBUG

void i::WindowProcedureThread()
{
    i::GetState()->win32.nativeThreadId = GetCurrentThreadId();

    std::wostringstream startMsg;
    startMsg << L"The window manager thread was started";
    i::Log(startMsg.str().c_str(), i::LlDebug);

    std::unique_lock<std::mutex> lock(i::GetState()->windowThreadMutex);
    i::GetState()->windowThreadIsRunning = true;
    lock.unlock();
    i::GetState()->windowThreadConditionVar.notify_one();

    // message pump for the window
    MSG msg{};
    while (GetMessageA(&msg, nullptr, 0, 0) != 0)
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);

        // DispatchMessage does not handle custom messages
        // It should theoretically only dismiss messages when in a modal loop but that's not the case
        if (msg.message >= WM_USER)
        {
            i::WindowProc(nullptr, msg.message, msg.wParam, msg.lParam);
        }
    }

    i::GetState()->windowThreadIsRunning = false;
    i::GetState()->windowThreadConditionVar.notify_one();
}

void i::CreateWin32Window(i::WindowData* wndDt)
{
    WIN32_EC_RET(wndDt->window, CreateWindowExW(
            0,
            i::GetState()->win32.pClassName,
            wndDt->name,
            WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
            // man do I love ternary expressions :)
            !wndDt->xPos ? CW_USEDEFAULT : wndDt->xPos, !wndDt->yPos ? CW_USEDEFAULT : wndDt->yPos,
            wndDt->width, wndDt->height,
            nullptr,
            nullptr,
            i::GetState()->win32.instance,
            nullptr
    ))

    ShowWindow(wndDt->window, 1);

    // ran out of range
    if (wndDt->id == SHRT_MAX)
    {
        i::Log(L"Handle out of range, window was opened but no handle could be created. You somehow opened 32767 "
               "windows...", i::LlError);
        return;
    }

    std::wostringstream oss;
    oss << L"Window " << wndDt->window << " was created and received a handle of " << wndDt->id;
    i::Log(oss.str().c_str(), i::LlDebug);

    i::GetState()->win32.identifiersToData.insert({wndDt->id, wndDt});
    i::GetState()->win32.handlesToData.insert({wndDt->window, wndDt});
}

LRESULT i::WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) // NOLINT(*-function-cognitive-complexity)
{
    switch (message)
    {
        ///////////////////////////////////
        // Custom messages
        case WM_CREATE_WINDOW_REQ:
        {
            i::CreateWin32Window(reinterpret_cast<WindowData*>(lParam));
            break;
        }
            // Custom messages
            ///////////////////////////////////

            ///////////////////////////////////
            // Closing the window
        case WM_CLOSE: // closing of a window has been requested
        {
            if (GetWindowData(window)->pfOnCloseAttempt()) // go ahead and close the window if return is true
                DestroyWindow(window);
            return 0;
        }
        case WM_DESTROY: // closing a window was ordered and confirmed
        {
            WindowData* wnd = GetWindowData(window);
            wnd->pfOnClose();
            wnd->isValid = false;
            i::GetState()->windowCount -= 1;
            i::EraseWindowData(window);

            if (i::GetState()->windowCount == 0) // quit program if last window remaining is closed
            {
                i::GetState()->isRunning = false;
                PostQuitMessage(0);
            }

            return 0;
        }
            // Closing the window
            ///////////////////////////////////

            ///////////////////////////////////
            // Keyboard and text input
        case WM_KEYDOWN:
        {
            if (!(lParam & 0x40000000))
            {
                i::Log("Key state was updated", i::LlInfo);
                i::GetState()->keyStates.set(wParam);
            }
            break;
        }
        case WM_KEYUP:
        {
            i::GetState()->keyStates.reset(wParam);
            break;
        }
        case WM_SYSKEYDOWN:
        {
            if (!(lParam & 0x40000000))
            {
                i::Log("Key state was updated", i::LlInfo);
                i::GetState()->keyStates.set(wParam);
            }
            break;
        }
        case WM_SYSKEYUP:
        {
            i::GetState()->keyStates.reset(wParam);
            break;
        }
        case WM_CHAR:
        {
            if (i::GetState()->textInputEnabled)
            {
                if (wParam != 0x0008) // key is not backspace
                {
                    auto* pText = i::GetState()->textInput;
                    size_t textLength = std::wcslen(pText);
                    if (textLength == 100000) break;
                    pText[textLength] = static_cast<wchar_t>(wParam);
                }
                else // key is backspace
                {
                    auto* pText = i::GetState()->textInput;
                    size_t textLength = std::wcslen(pText);
                    pText[textLength - 1] = 0;
                }
            }
            break;
        }
            // Keyboard and text input
            ///////////////////////////////////

            ///////////////////////////////////
            // Mouse input
        case WM_MOUSEMOVE:
        {
            i::GetState()->mouse.xPos = GET_X_LPARAM(lParam);
            i::GetState()->mouse.yPos = GET_Y_LPARAM(lParam);
            GetWindowData(window)->hasMouseInClientArea = true;

            break;
        }
        case WM_LBUTTONDOWN:
        {
            SetCapture(window);
            i::GetState()->mouse.leftButton = true;
            break;
        }
        case WM_LBUTTONUP:
        {
            ReleaseCapture();
            i::GetState()->mouse.leftButton = false;
            break;
        }
        case WM_MBUTTONDOWN:
        {
            SetCapture(window);
            i::GetState()->mouse.middleButton = true;
            break;
        }
        case WM_MBUTTONUP:
        {
            i::GetState()->mouse.middleButton = false;
            break;
        }
        case WM_RBUTTONDOWN:
        {
            i::GetState()->mouse.rightButton = true;
            break;
        }
        case WM_RBUTTONUP:
        {
            i::GetState()->mouse.rightButton = false;
            break;
        }
        case WM_XBUTTONDOWN:
        {
            int button = HIWORD(wParam);
            switch (button)
            {
                case 1:
                {
                    i::GetState()->mouse.x1Button = true;
                    break;
                }
                case 2:
                {
                    i::GetState()->mouse.x2Button = true;
                    break;
                }
                default: break;
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
                    i::GetState()->mouse.x1Button = false;
                    break;
                }
                case 2:
                {
                    i::GetState()->mouse.x2Button = false;
                    break;
                }
                default: break;
            }
            break;
        }
        case WM_MOUSEWHEEL:
        {
            i::GetState()->mouse.wheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
            break;
        }
        case WM_MOUSELEAVE:
        {
            GetWindowData(window)->hasMouseInClientArea = false;
            break;
        }
            // Mouse input
            ///////////////////////////////////

            ///////////////////////////////////
            // Focus gain and loss
        case WM_SETFOCUS:
        {
            WindowData* wndData = i::GetWindowData(window);
            if (wndData)
            {
                wndData->hasFocus = true;
            }

            break;
        }
        case WM_KILLFOCUS:
        {
            WindowData* wndData = i::GetWindowData(window);
            if (wndData)
            {
                wndData->hasFocus = false;
            }
            i::GetState()->keyStates.reset();
            break;
        }
            // Focus gain and loss
            ///////////////////////////////////

            // Yes Clang-Tidy, I know
        default: break;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}
#endif // _WINDOWS
