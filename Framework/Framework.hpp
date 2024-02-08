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

// STL
#include <complex>
#include <vector>


namespace f // a one letter namespace name...
{
    // Keys used for keyboard related functions
    // Basically a forward of the Win32 symbols
    // And yes I had to hand write this
    enum Key
    {
        K_LBUTTON = 0x01, // left mouse button
        K_RBUTTON = 0x02, // right mouse button
        K_CANCEL = 0x03, // cancel key
        K_MBUTTON = 0x04, // middle mouse button
        K_XBUTTON1 = 0x05, // extra mouse button 1, usually on the side
        K_XBUTTON2 = 0x06, // extra mouse button 2, usually on the side

        // 0x07 is reserved

        K_BACK = 0x08, // the one with the left arrow on it
        K_TAB = 0x09, // tab

        // 0x0A and 0x0B are reserved

        K_CLEAR = 0x0C, // clear
        K_ENTER = 0x0D, // return or enter

        // 0x0E and 0x0F are reserved

        K_SHIFT = 0x10, // shift
        K_CONTROLL = 0x11, // controll
        K_ALT = 0x12, // both alt keys
        K_PAUSE = 0x13, // pause
        K_CAPS = 0x14, // caps, though I have no idea why you would use this
        K_KANA = 0x15, // ime kana mode
        K_HANGUL = 0x15, // ime hangul mode
        K_IME_ON = 0x16, // turn ime on
        K_JUNJA = 0x17, // ime junja mode
        K_FINAL = 0x18, // ime final mode
        K_HANJA = 0x19, // ime hanja mode
        K_KANJI = 0x19, // ime kanji mode
        K_IME_OFF = 0x1A, // turn ime off
        K_ESCAPE = 0x1B, // escape
        K_CONVERT = 0x1C, // ime convert
        K_NONCONVERT = 0x1D, // ime not convert
        K_ACCEPT = 0x1E, // ime accept
        K_MODECHANGE = 0x1F, // ime change mode
        K_SPACE = 0x20, // space, that big one in the middle
        K_PRIOR = 0x21, // page up
        K_NEXT = 0x22, // page down
        K_END = 0x23, // end
        K_HOME = 0x24, // home
        K_LEFT = 0x25, // left arrow key
        K_UP = 0x26, // up arrow key
        K_RIGHT = 0x27, // right arrow key
        K_DOWN = 0x28, // down arrow key
        K_SELECT = 0x29, // select
        K_PRINT = 0x2A, // print
        K_EXECUTE = 0x2B, // hmmm ... where could this one be?
        K_SNAPSHOT = 0x2C, // another key not present on modern keyboards
        K_INSERT = 0x2D, // insert
        K_DELETE = 0x2E, // delete
        K_HELP = 0x2F, // help, sent when F1 is pressed
        K_BAR_0 = 0x30, // number 0 in the top number bar
        K_BAR_1 = 0x31, // number 1 in the top number bar
        K_BAR_2 = 0x32, // number 2 in the top number bar
        K_BAR_3 = 0x33, // number 3 in the top number bar
        K_BAR_4 = 0x34, // number 4 in the top number bar
        K_BAR_5 = 0x35, // number 5 in the top number bar
        K_BAR_6 = 0x36, // number 6 in the top number bar
        K_BAR_7 = 0x37, // number 7 in the top number bar
        K_BAR_8 = 0x38, // number 8 in the top number bar
        K_BAR_9 = 0x39, // number 9 in the top number bar

        // 0x3A to 0x40 is undefined / unused

        K_A = 0x41,
        K_B, K_C, K_D, K_E, K_F, K_G, K_H, K_I, K_J, K_K, K_L, K_M, K_N, K_O, K_P, K_Q, K_R, K_S, K_T, K_U, K_V, K_W, K_X, K_Y,
        K_Z = 0x5A,
        K_LWIN = 0x5B, // left windows key
        K_RWIN = 0x5C, // right windows key
        K_APPS = 0x5D, // applications key

        // 0x5E is reserved

        K_SLEEP = 0x5F, // sleep, I am not sure if this is ever going to be used
        K_PAD_0 = 0x60, // number 0 on the keypad
        K_PAD_1 = 0x61, // number 1 on the keypad
        K_PAD_2 = 0x62, // number 2 on the keypad
        K_PAD_3 = 0x63, // number 3 on the keypad
        K_PAD_4 = 0x64, // number 4 on the keypad
        K_PAD_5 = 0x65, // number 5 on the keypad
        K_PAD_6 = 0x66, // number 6 on the keypad
        K_PAD_7 = 0x67, // number 7 on the keypad
        K_PAD_8 = 0x68, // number 8 on the keypad
        K_PAD_9 = 0x69, // number 9 on the keypad
        K_MULTIPLY = 0x6A, // multiply key on the keypad
        K_ADD = 0x6B, // add key on the keyboard
        K_SEPERATOR = 0x6C, // seperator
        K_SUBTRACT = 0x6D, // subtract key on the keypad
        K_DECIMAL = 0x6E, // decimal, probably also on the keypad
        K_DEVIDE = 0x6F, // devide key on the keypad
        K_F1 = 0x70,
        K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_F11, K_F12, K_F13, K_F14, K_F15, K_F16, K_F17, K_F18,
        K_F19, K_F20, K_F21, K_F22, K_F23,
        K_F24 = 0x87,

        // 0x88 to 0x8F is reserved

        K_NUMLOCK = 0x90, // literally why?
        K_SCROLL = 0x91, // scroll, not the one on the mouse

        // 0x92 to 9x96 is OEM specific and therefore unusable
        // 0x97 to 0x9F is unsassigned / unused

        K_LSHIFT = 0xA0, // left shift
        K_RSHIFT = 0xA1, // right shift
        K_LCONTROL = 0xA2, // left control
        K_RCONTROL = 0xA3, // right control
        K_LALT = 0xA4, // left alt
        K_RALT = 0xA5, // right alt
        K_BROWSER_BACK = 0xA6, // browser back, yep ... no clue either
        K_BROWSER_FORWARD = 0xA7, // browser forward
        K_BROWSER_REFRESH = 0xA8, // browser refresh
        K_BROWSER_STOP = 0xA9, // browser stop
        K_BROWSER_SEARCH = 0xAA, // browser search 
        K_BROWSER_FAV = 0xAB, // browser favorites
        K_BROWSER_HOME = 0xAC, // browser home
        K_VOLUME_MUTE = 0xAD, // volume key, mute system sound
        K_VOLUME_DOWN = 0xAE, // volume key, make system sound quieter
        K_VOLUME_UP = 0xAF, // volume key, make system sound louder
        K_MEDIA_NEXT = 0xB0, // media key, play next track
        K_MEDIA_PREV = 0xB1, // media key, play previous track
        K_MEDIA_STOP = 0xB2, // media key, quit playback
        K_MEDIA_PLAY_PAUSE = 0xB3, // media key, continue playback or pause
        K_LAUNCH_MAIL = 0xB4, // launch mail
        K_MEDIA_SELECT = 0xB5, // select media key
        K_LAUNCH1 = 0xB6, // launch key 1, keyboard manufacturer specific
        K_LAUNCH2 = 0xB7, // launch key 2, keyboard manufacturer specific

        // 0xB8 and 0xB9 are reserved

        K_OEM1 = 0xBA, // misc characters, varies by keyboard layout (on US-layout: ";" or ":")
        K_PLUS_OEM = 0xBB, // the + key for any layout
        K_COMMA_OEM = 0xBC, // the , key for any layout
        K_MINUS_OEM = 0xBD, // the - key for any layout
        K_PERIOS_OEM = 0xBE, // the . key for any layout
        K_OEM2 = 0xBF, // misc characters, varies by keyboard layout (on US-layout: "/" or "?")
        K_OEM3 = 0xC0, // misc characters, varies by keyboard layout (on US-layout: "`" or "~")

        // 0xC1 to 0xDA is reserved

        K_OEM4 = 0xDB, // misc characters, varies by keyboard layout (on US-layout: "[" or "{")
        K_OEM5 = 0xDC, // misc characters, varies by keyboard layout (on US-layout: "\\" or "|")
        K_OEM6 = 0xDD, // misc characters, varies by keyboard layout (on US-layout: "]" or "}")
        K_OEM7 = 0xDE, // misc characters, varies by keyboard layout (on US-layout: "'" or """)
        K_OEM8 = 0xDF, // misc characters, varies by keyboard layout

        // 0xE0 is reserved and 0xE1 is OEM specific

        K_OEM102 = 0xE2, // misc characters, varies by keyboard layout (on US102-layout: "<" or ">", US-non-102: "\\" or "|")

        // 0xE23 and 0xE4 are OEM specific

        K_PROCESSKEY = 0xE5, // ime process key

        // 0xE6 to F5 is a bunch of OEM specific and unused stuff

        K_ATTN = 0xF6, // attn key
        K_CRSEL = 0xF7, // CrSel key
        K_EXSEL = 0xF8, // ExSel key
        K_EREOF = 0xF9, // erase end-of-file key
        K_PLAY = 0xFA, // playback
        K_ZOOM = 0xFB, // change zoom level
        K_NONAME = 0xFC, // unused but still valid key
        K_PA1 = 0xFD, // pa1 key
        K_CLEAR_OEM = 0xFE  // oem specific clear key
    };

    // Messagebox flags
    // biggest possible size is 3 bytes + 1 bit, passed as 4 byte int
    enum MF
    {
        // Block execution as long as the message box is open
        MB_TASKMODAL                        = 0b1,

        // Icons
        MB_ICON_WARNING                     = 0b10,
        MB_ICON_ERROR                       = 0b100,
        MB_ICON_INFO                        = 0b1000,
        MB_ICON_QUESTION                    = 0b10000,

        // Buttons, 2 options
        MB_BUTTON_OK                        = 0b100000,
        MB_BUTTON_OK_CANCEL                 = 0b1000000,
        MB_BUTTON_YES_NO                    = 0b10000000,
        MB_BUTTON_RETRY_CANEL               = 0b100000000,

        // Buttons, 3 options
        MB_BUTTON_YES_NO_CANCEL             = 0b1000000000,
        MB_BUTTON_ABORT_RETRY_IGNORE        = 0b100000000000,
        MB_BUTTON_CANCEL_RETRY_CONTINUE     = 0b1000000000000
    };

    // Message box return
    // This is the meaning of the return value gotten from CreateMessageBox
    // Basically just the button that was pressed
    enum MB
    {
        MB_ABORT = 1,
        MB_CANCEL,
        MB_CONTINUE,
        MB_IGNORE,
        MB_NO,
        MB_OK,
        MB_RETRY,
        MB_TRYAGAIN,
        MB_YES
    };

    // Window position relation
    // Used for the WindowGetPosition functions
    enum WP
    {
        // WindowGetXPos
        WP_LEFT = 1,
        WP_RIGHT,

        // WindowGetYPos
        WP_TOP,
        WP_BOTTOM,

        // WindowGetPosition
        WP_TOP_LEFT,
        WP_TOP_RIGHT,
        WP_BOTTOM_LEFT,
        WP_BOTTOM_RIGHT
    };

    typedef short WND_H;

    struct INITIALIZATION_DATA
    {
        int iconId, cursorId;
    };

    struct WindowCreateData
    {
        unsigned int width, height;
        int xPos, yPos;
        const wchar_t* pName;
        std::vector<WND_H> dependants;

        void (*fpOnClose)();
        bool (*fpOnCloseAttempt)();
    };

    // Start the entirety of this framework up so it can be used.
    // Can fail if a resource id is invalid
    void Initialize(int icon, int cursor);

    // Shutdown, cleanup.
    // Only call when it is garanteed that all windows are closed.
    void Uninitialize();

    // Create a new window 
    WND_H CreateWindow(const f::WindowCreateData& wndCrtData);

    // Close a window
    void CloseWindow(const WND_H handle);

    // Close all windows
    void CloseAllWindows();

    // Specify a function to be executed when a window is requested to be closed
    // The function must return a boolean indicating wheather the window should be closed
    void OnWindowCloseAttempt(WND_H handle, bool(*func)(void));

    // Specify a function to be executed when a window is closed
    void OnWindowClose(WND_H handle, void(*func)(void));

    // Returns the name of the window which matches the given handle
    // This function can fail if the handle is not valid
    wchar_t* GetWindowName(WND_H id);

    // Returns the state of visibility of the window which matches the given handle
    // This function can fail if the handle is not valid
    bool GetWindowVisibility(WND_H id);

    // Returns the width of the window which matches the given handle
    // This function can fail if the handle is not valid
    int GetWindowWidth(WND_H id);

    // Returns the height of the window which matches the given handle
    // This function can fail if the handle is not valid
    int GetWindowHeight(WND_H id);

    // Returns the window size
    // This function can fail if the handle is not valid
    std::pair<int, int> GetWindowDimensions(WND_H id);

    // Returns the window positions on the x-axis
    // This function can fail if the handle is invalid
    int GetWindowXPos(WND_H id, WP wpr);

    // Returns the window position on the y-axis
    // This function can fail if the handle is invalid
    int GetWindowYPos(WND_H id, WP wpr);

    // Returns the distance from the top-left corner from the display to the specified corner of the window
    // This function can fail if the handle is invalid
    std::pair<int, int> GetWindowPosition(WND_H id, WP wpr);

    // Returns the number of open windows
    int GetWindowCount();

    // Changes the name of the specified window to the given name
    // Can fail if the handle is invalid
    bool ChangeWindowName(WND_H id, const wchar_t* name);

    // Return whether the specified window has keyboard focus or not
    bool WindowHasFocus(WND_H id);

    // Returns whether the passed handle corresponds to an existing window or not
    bool IsValidHandle(WND_H handle);

    // Returns true as long as any window is open
    bool Running();

    // Pause the caller thread for the specified ammout of ms
    void Halt(int ms);

    // Creates a message box with the given information
    // Can only have one owner
    int MessageBox(WND_H owner, const wchar_t* title, const wchar_t* msg, int flags);

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
    bool WindowContainsMouse(WND_H handle);

    // Returns a handle to the window that currently contains the mouse. This is independent from focus.
    // If none of your window contains the mouse it will return null and set an error.
    // You may choose to ignore the error if intended as it is insignificant.
    WND_H GetMouseContainerWindow();
}
