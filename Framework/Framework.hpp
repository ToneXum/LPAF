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

// Automatic error handling, the return value is thrown away
#define TSD_CALL(callee, q) { if (!callee) { tsd::CreateAutoError(__LINE__, q); } }

// Automatic errror handling, the return value is saved
#define TSD_CALL_RET(ret, callee, q) { ret = callee; if (!ret) { tsd::CreateAutoError(__LINE__, q); } }

namespace tsd // tonexum software division
{
    // Messagebox flags
    // biggest possible size is 3 bytes + 1 bit, passed as 4 byte int
    enum class MBF
    {
        // Block execution as long as the message box is open
        TASKMODAL                       = 0b1,

        // Icons
        ICON_WARNING                    = 0b10,
        ICON_ERROR                      = 0b100,
        ICON_INFO                       = 0b1000,
        ICON_QUESTION                   = 0b10000,

        // Buttons, 2 options
        BUTTON_OK                       = 0b100000,
        BUTTON_OK_CANCEL                = 0b1000000,
        BUTTON_YES_NO                   = 0b10000000,
        BUTTON_RETRY_CANEL              = 0b100000000,

        // Buttons, 3 options
        BUTTON_YES_NO_CANCEL            = 0b1000000000,
        BUTTON_ABORT_RETRY_IGNORE       = 0b100000000000,
        BUTTON_CANCEL_RETRY_CONTINUE    = 0b1000000000000
    };

    // Message box return
    // This is the meaning of the return value gotten from CreateMessageBox
    // Basically just the button that was pressed
    enum class MBR
    {
        ABORT = 1,
        CANCEL,
        CONTINUE,
        IGNORE,
        NO,
        OK,
        RETRY,
        TRYAGAIN,
        YES
    };

    // Window position relation
    // Used for the WindowGetPosition functions
    enum class WPR
    {
        // WindowGetXPos
        LEFT = 1,
        RIGHT,

        // WindowGetYPos
        TOP,
        BOTTOM,

        // WindowGetPosition
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_RIGHT
    };

    // Keys used for keyboard related functions
    // Basically a forward of the Win32 symbols
    // And yes I had to hand write this
    enum class Key
    {
        LBUTTON             = 0x01, // left mouse button
        RBUTTON             = 0x02, // right mouse button
        CANCEL              = 0x03, // cancel key
        MBUTTON             = 0x04, // middle mouse button
        XBUTTON1            = 0x05, // extra mouse button 1, usually on the side
        XBUTTON2            = 0x06, // extra mouse button 2, usually on the side

        // 0x07 is reserved

        BACK                = 0x08, // the one with the left arrow on it
        TAB                 = 0x09, // tab

        // 0x0A and 0x0B are reserved

        CLEAR               = 0x0C, // clear
        ENTER               = 0x0D, // return or enter

        // 0x0E and 0x0F are reserved

        SHIFT               = 0x10, // shift
        CONTROLL            = 0x11, // controll
        ALT                 = 0x12, // both alt keys
        PAUSE               = 0x13, // pause
        CAPS                = 0x14, // caps, though I have no idea why you would use this
        KANA                = 0x15, // ime kana mode
        HANGUL              = 0x15, // ime hangul mode
        IME_ON              = 0x16, // turn ime on
        JUNJA               = 0x17, // ime junja mode
        FINAL               = 0x18, // ime final mode
        HANJA               = 0x19, // ime hanja mode
        KANJI               = 0x19, // ime kanji mode
        IME_OFF             = 0x1A, // turn ime off
        ESCAPE              = 0x1B, // escape
        CONVERT             = 0x1C, // ime convert
        NONCONVERT          = 0x1D, // ime not convert
        ACCEPT              = 0x1E, // ime accept
        MODECHANGE          = 0x1F, // ime change mode
        SPACE               = 0x20, // space, that big one in the middle
        PRIOR               = 0x21, // page up
        NEXT                = 0x22, // page down
        END                 = 0x23, // end
        HOME                = 0x24, // home
        LEFT                = 0x25, // left arrow key
        UP                  = 0x26, // up arrow key
        RIGHT               = 0x27, // right arrow key
        DOWN                = 0x28, // down arrow key
        SELECT              = 0x29, // select
        PRINT               = 0x2A, // print
        EXECUTE             = 0x2B, // hmmm ... where could this one be?
        SNAPSHOT            = 0x2C, // another key not present on modern keyboards
        INSERT              = 0x2D, // insert
        DELETE              = 0x2E, // delete
        HELP                = 0x2F, // help, sent when F1 is pressed
        BAR_0               = 0x30, // number 0 in the top number bar
        BAR_1               = 0x31, // number 1 in the top number bar
        BAR_2               = 0x32, // number 2 in the top number bar
        BAR_3               = 0x33, // number 3 in the top number bar
        BAR_4               = 0x34, // number 4 in the top number bar
        BAR_5               = 0x35, // number 5 in the top number bar
        BAR_6               = 0x36, // number 6 in the top number bar
        BAR_7               = 0x37, // number 7 in the top number bar
        BAR_8               = 0x38, // number 8 in the top number bar
        BAR_9               = 0x39, // number 9 in the top number bar

        // 0x3A to 0x40 is undefined / unused

        A                   = 0x41,
        B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y,
        Z                   = 0x5A,
        LWIN                = 0x5B, // left windows key
        RWIN                = 0x5C, // right windows key
        APPS                = 0x5D, // applications key

        // 0x5E is reserved

        SLEEP               = 0x5F, // sleep, I am not sure if this is ever going to be used
        PAD_0               = 0x60, // number 0 on the keypad
        PAD_1               = 0x61, // number 1 on the keypad
        PAD_2               = 0x62, // number 2 on the keypad
        PAD_3               = 0x63, // number 3 on the keypad
        PAD_4               = 0x64, // number 4 on the keypad
        PAD_5               = 0x65, // number 5 on the keypad
        PAD_6               = 0x66, // number 6 on the keypad
        PAD_7               = 0x67, // number 7 on the keypad
        PAD_8               = 0x68, // number 8 on the keypad
        PAD_9               = 0x69, // number 9 on the keypad
        MULTIPLY            = 0x6A, // multiply key on the keypad
        ADD                 = 0x6B, // add key on the keyboard
        SEPERATOR           = 0x6C, // seperator
        SUBTRACT            = 0x6D, // subtract key on the keypad
        DECIMAL             = 0x6E, // decimal, probably also on the keypad
        DEVIDE              = 0x6F, // devide key on the keypad
        F1                  = 0x70,
        F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23,
        F24                 = 0x87,

        // 0x88 to 0x8F is reserved

        NUMLOCK             = 0x90, // literally why?
        SCROLL              = 0x91, // scroll, not the one on the mouse

        // 0x92 to 9x96 is OEM specific and therefore unusable
        // 0x97 to 0x9F is unsassigned / unused

        LSHIFT              = 0xA0, // left shift
        RSHIFT              = 0xA1, // right shift
        LCONTROL            = 0xA2, // left control
        RCONTROL            = 0xA3, // right control
        LALT                = 0xA4, // left alt
        RALT                = 0xA5, // right alt
        BROWSER_BACK        = 0xA6, // browser back, yep ... no clue either
        BROWSER_FORWARD     = 0xA7, // browser forward
        BROWSER_REFRESH     = 0xA8, // browser refresh
        BROWSER_STOP        = 0xA9, // browser stop
        BROWSER_SEARCH      = 0xAA, // browser search 
        BROWSER_FAV         = 0xAB, // browser favorites
        BROWSER_HOME        = 0xAC, // browser home
        VOLUME_MUTE         = 0xAD, // volume key, mute system sound
        VOLUME_DOWN         = 0xAE, // volume key, make system sound quieter
        VOLUME_UP           = 0xAF, // volume key, make system sound louder
        MEDIA_NEXT          = 0xB0, // media key, play next track
        MEDIA_PREV          = 0xB1, // media key, play previous track
        MEDIA_STOP          = 0xB2, // media key, quit playback
        MEDIA_PLAY_PAUSE    = 0xB3, // media key, continue playback or pause
        LAUNCH_MAIL         = 0xB4, // launch mail
        MEDIA_SELECT        = 0xB5, // select media key
        LAUNCH1             = 0xB6, // launch key 1, keyboard manufacturer specific
        LAUNCH2             = 0xB7, // launch key 2, keyboard manufacturer specific

        // 0xB8 and 0xB9 are reserved

        OEM1                = 0xBA, // misc characters, varies by keyboard layout (on US-layout: ";" or ":")
        PLUS_OEM            = 0xBB, // the + key for any layout
        COMMA_OEM           = 0xBC, // the , key for any layout
        MINUS_OEM           = 0xBD, // the - key for any layout
        PERIOS_OEM          = 0xBE, // the . key for any layout
        OEM2                = 0xBF, // misc characters, varies by keyboard layout (on US-layout: "/" or "?")
        OEM3                = 0xC0, // misc characters, varies by keyboard layout (on US-layout: "`" or "~")

        // 0xC1 to 0xDA is reserved

        OEM4                = 0xDB, // misc characters, varies by keyboard layout (on US-layout: "[" or "{")
        OEM5                = 0xDC, // misc characters, varies by keyboard layout (on US-layout: "\\" or "|")
        OEM6                = 0xDD, // misc characters, varies by keyboard layout (on US-layout: "]" or "}")
        OEM7                = 0xDE, // misc characters, varies by keyboard layout (on US-layout: "'" or """)
        OEM8                = 0xDF, // misc characters, varies by keyboard layout

        // 0xE0 is reserved and 0xE1 is OEM specific

        OEM102              = 0xE2, // misc characters, varies by keyboard layout (on US102-layout: "<" or ">", US-non-102: "\\" or "|")

        // 0xE23 and 0xE4 are OEM specific

        PROCESSKEY          = 0xE5, // ime process key

        // 0xE6 to F5 is a bunch of OEM specific and unused stuff

        ATTN                = 0xF6, // attn key
        CRSEL               = 0xF7, // CrSel key
        EXSEL               = 0xF8, // ExSel key
        EREOF               = 0xF9, // erase end-of-file key
        PLAY                = 0xFA, // playback
        ZOOM                = 0xFB, // change zoom level
        NONAME              = 0xFC, // unused but still valid key
        PA1                 = 0xFD, // pa1 key
        CLEAR_OEM           = 0xFE  // oem specific clear key
    };

    // Start the entirety of this framework up so it can be used.
    // Can fail if a resource id is invalid
    bool Initialise(int icon, int cursor);

    // Shutdown, cleanup.
    // Only call when it is garanteed that all windows are closed. Otherwise it will get the execution of the
    // Caller stuck
    void Uninitialise();

    // Automatic user error handling
    // Use this if you are lazy
    void CreateAutoError(int line, bool quit);

    // If an error occours, the callee returns null. Read the error code with this function
    // Beware only check the return value of functions that actually set errors
    int GetLastError();

    // Translates the error by code into a readable message
    const char* GetErrorInformation(int code);

    // Create a new window 
    short CreateWindow(const wchar_t* name, int width, int height, int xPos = 0, int yPos = 0);

    // Returns the name of the window which matches the given handle
    // This function can fail if the handle is not valid
    wchar_t* GetWindowName(short id);

    // Returns the state of visibility of the window which matches the given handle
    // This function can fail if the handle is not valid
    bool GetWindowVisibility(short id);

    // Returns the width of the window which matches the given handle
    // This function can fail if the handle is not valid
    int GetWindowWidth(short id);

    // Returns the height of the window which matches the given handle
    // This function can fail if the handle is not valid
    int GetWindowHeight(short id);

    // Returns the window size
    // This function can fail if the handle is not valid
    std::pair<int, int> GetWindowDimensions(short id);

    // Returns the window positions on the x-axis
    // This function can fail if the handle is invalid
    int GetWindowXPos(short id, WPR wpr);

    // Returns the window position on the y-axis
    // This function can fail if the handle is invalid
    int GetWindowYPos(short id, WPR wpr);

    // Returns the distance from the top-left corner from the display to the specified corner of the window
    // This function can fail if the handle is invalid
    std::pair<int, int> GetWindowPosition(short id, WPR wpr);

    // Returns the number of open windows
    int GetWindowCount();

    // Changes the name of the specified window to the given name
    // Can fail if the handle is invalid
    bool ChangeWindowName(short id, const wchar_t* name);

    // Return whether the specified window has keyboard focus or not
    bool WindowHasFocus(short id);

    // Returns whether the passed handle is valid or not
    bool IsValidHandle(short handle);

    // Returns true as long as any window is open
    bool Running();

    // Pause the caller thread for the specified ammout of ms
    void Halt(int ms);

    // Creates a message box with the given information
    // Can only have one owner
    MBR MessageBox(short owner, const wchar_t* title, const wchar_t* msg, int flags);

    // Check the keystate for the specified key, return true if the key is pressed
    bool IsKeyPressed(Key code);

    // Check the keystate for the specified key, return true if the key is pressed
    // The function being sticky refers to the reset of the keystate that is being checked. When the checked key is
    // pressed, the function will reset the internal keystate even while the physical button is still being pressed.
    // Quite usefull for toggle-like behaviour where the key is being held down for multiple calls
    bool IsKeyPressedOnce(Key code);

    // Returns true if the specified key is released.
    // This function must be executed in a loop since it stores the keystate at the last call.
    bool IsKeyReleased(Key code);

    // Returns true if any key is pressed.
    bool IsAnyKeyPressed();

    // Enables / disables the text input field.
    void SetTextInputState(bool state, bool clear = true);

    // Returns a pointer to the text input field.
    wchar_t* GetTextInput();

    // Clears the text input field out to all 0's.
    void ClearTextInput();

    // Returns whether the text input field is enabled or not.
    bool IsTextInputEnabled();

    struct MouseInfo
    {
        int xPos, yPos;
        bool left, right, middle, x1, x2;
        short containingWindow;
    };

    // Returns a struct containing all information about the state of the mouse.
    // Usefull when checking multiple factors of the mouse since you wont have to make multiple function calls.
    MouseInfo GetMouseInfo();

    // Returns the mouse position on the x coordinate.
    int GetMouseX();

    // Returns the mouse position on the y coordinate.
    int GetMouseY();

    // Returns true when the right mouse button is pressed any your application has focus.
    bool GetMouseLeftButton();

    bool GetMouseLeftButtonOnce();

    // Returns true when the right mouse button is pressed any your application has focus.
    bool GetMouseRightButton();

    bool GetMouseRightButtonOnce();

    // Returns true when the middle mouse button is pressed any your application has focus.
    bool GetMouseMiddleButton();

    bool GetMouseMiddleButtonOnce();

    // Returns true when the first extra mouse button is pressed any your application has focus.
    bool GetMouseX1Button();

    bool GetMouseX1ButtonOnce();

    // Returns true when the second extra mouse button is pressed any your application has focus.
    bool GetMouseX2Button();

    bool GetMouseX2ButtonOnce();

    // Returns the number of detents the mouse wheel was scrolled compared to the last time this function was called.
    // A negative value means a scroll down, a positive one means a scroll up. Null means the wheel has not moved.
    // Must be executed in a loop.
    int GetMouseWheelDelta();

    // Returns true if the mouse is hovering over the specified window. It does not matter if the window has focus or not
    // Always returns false if the handle is invalid.
    bool WindowContainsMouse(short handle);

    // Returns a handle to the window that currently contains the mouse. This is independent from focus.
    // If none of your window contains the mouse it will return null and set an error.
    // You may choose to ignore the error if intended as it is insignificant.
    short GetMouseContainerWindow();
}
