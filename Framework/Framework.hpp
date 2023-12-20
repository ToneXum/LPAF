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

#pragma once

#include <complex>
#include "Exposed-Globals.hpp"

// Displays debug information depending on the build type
#ifdef _DEBUG
// Automatic error handling, the return value is thrown away
#define TSD_CALL(callee, q) { if (!callee) { tsd::CreateAutoDebugError(__LINE__, q); } }

// Automatic errror handling, the return value is saved
#define TSD_CALL_RET(ret, callee, q) { ret = callee; if (!ret) { tsd::CreateAutoDebugError(__LINE__, q); } }
#endif
#ifdef NDEBUG
// Automatic error handling, the return value is thrown away
#define TSD_CALL(callee, q) { if (!callee) { tsd::CreateAutoReleaseError(__LINE__, q); } }

// Automatic errror handling, the return value is saved
#define TSD_CALL_RET(ret, callee, q) { ret = callee; if (!ret) { tsd::CreateAutoReleaseError(__LINE__, q); } }
#endif // NDEBUG

namespace tsd // tonexum software division
{
    // Start the entirety of this framework up so it can be used.
    // Can fail if a resource id is invalid
    bool Initialise(int icon, int cursor);

    // Shutdown, cleanup.
    // Only call when it is garanteed that all windows are closed. Otherwise it will get the execution of the
    // Caller stuck
    void Uninitialise();

    // Automatic user error handling
    // Use this if you are lazy
    void CreateAutoDebugError(int line, bool quit);

    // UNIMPLEMENTED!!
    // Automatic user error handling
    // Use this if you are lazy
    void CreateAutoReleaseError(int line, bool quit); 

    // If an error occours, the callee returns null. Read the error code with this function
    // Beware only check the return value of functions that actually set errors
    int GetLastError();

    // Translates the error by code into a readable message
    const char* GetErrorInformation(int code);

    // Create a new window 
    short CreateWindow(const wchar_t* name, int width, int height, int xPos = 0, int yPos = 0);

    // Returns the name of the window which matches the given handle
    // This function can fail if the handle is not valid
    wchar_t* WindowGetName(short id);

    // Returns the state of visibility of the window which matches the given handle
    // This function can fail if the handle is not valid
    bool WindowGetVisibility(short id);

    // Returns the width of the window which matches the given handle
    // This function can fail if the handle is not valid
    int WindowGetWidth(short id);

    // Returns the height of the window which matches the given handle
    // This function can fail if the handle is not valid
    int WindowGetHeight(short id);

    // Returns the window size
    // This function can fail if the handle is not valid
    std::pair<int, int> WindowGetDimensions(short id);

    // Returns the window positions on the x-axis
    // This function can fail if the handle is invalid
    int WindowGetXPos(short id, WPR wpr);

    // Returns the window position on the y-axis
    // This function can fail if the handle is invalid
    int WindowGetYPos(short id, WPR wpr);

    // Returns the distance from the top-left corner from the display to the specified corner of the window
    // This function can fail if the handle is invalid
    std::pair<int, int> WindowGetPosition(short id, WPR wpr);

    // Returns the number of open windows
    int WindowGetCount();

    // Changes the name of the specified window to the given name
    // Can fail if the handle is invalid
    bool WindowChangeName(short id, const wchar_t* name);

    // Returns whether the passed handle is valid or not
    bool IsValidHandle(short handle);

    // Returns true as long as any window is open
    bool Running();

    // Pause the caller thread for the specified ammout of ms
    void Halt(int ms);

    // Creates a message box with the given information
    // Can only have one owner
    MBR MessageBox(short owner, const wchar_t* title, const wchar_t* msg, int flags);

    // Check the keystate for the specified key, return true if the the key is pressed
    bool IsKeyPressed(Key code);

    // Returns true if any key is pressed
    bool IsAnyKeyPressed();

    // Enables / disables the text input field
    void SetTextInputState(bool state, bool clear = true);

    // Returns a pointer to the text input field
    wchar_t* GetTextInput();

    // Clears the text input field out to all 0's
    void ClearTextInput();

    // Returns whether the text input field is enabled or not
    bool IsTextInputEnabled();
}
