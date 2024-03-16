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

// STL
#include <complex>
#include <cstdint>
#include <vector>

namespace f // a one letter namespace name...
{
// Handle to identify a window with
using WndH = uint16_t;

// Key identifiers you can pass to IsKeyPressed or IsKeyPressedOnce
enum Key : uint8_t
{
    KeyLButton             = 0x01, // left mouse button
    KeyRButton             = 0x02, // right mouse button
    KeyCancel              = 0x03, // cancel key
    KeyMButton             = 0x04, // middle mouse button
    KeyXButton1            = 0x05, // extra mouse button 1, usually on the side
    KeyXButton2            = 0x06, // extra mouse button 2, usually on the side

    // 0x07 is reserved

    KeyBack                = 0x08, // the one with the left arrow on it
    KeyTab                 = 0x09, // tab

    // 0x0A and 0x0B are reserved

    KeyClear               = 0x0C, // clear
    KeyEnter               = 0x0D, // return or enter

    // 0x0E and 0x0F are not assigned to anything

    KeyShift               = 0x10, // shift
    KeyControl             = 0x11, // control
    KeyAlt                 = 0x12, // both alt keys
    KeyPause               = 0x13, // pause
    KeyCaps                = 0x14, // caps, though I have no idea why you would use this
    KeyKana                = 0x15, // ime kana mode
    KeyHangul              = 0x15, // ime hangul mode
    KeyImeOn               = 0x16, // turn ime on
    KeyJunja               = 0x17, // ime junja mode
    KeyFinal               = 0x18, // ime final mode
    KeyHanja               = 0x19, // ime hanja mode
    KeyKanji               = 0x19, // ime kanji mode
    KeyImeOff              = 0x1A, // turn ime off
    KeyEscape              = 0x1B, // escape
    KeyConvert             = 0x1C, // ime convert
    KeyNonConvert          = 0x1D, // ime not convert
    KeyAccept              = 0x1E, // ime accept
    KeyModeChange          = 0x1F, // ime change mode
    KeySpace               = 0x20, // space, that big one in the middle
    KeyPrior               = 0x21, // page up
    KeyNext                = 0x22, // page down
    KeyEnd                 = 0x23, // end
    KeyHome                = 0x24, // home
    KeyLeft                = 0x25, // left arrow key
    KeyUp                  = 0x26, // up arrow key
    KeyRight               = 0x27, // right arrow key
    KeyDown                = 0x28, // down arrow key
    KeySelect              = 0x29, // select
    KeyPrint               = 0x2A, // print
    KeyExecute             = 0x2B, // hmm ... where could this one be?
    KeySnapshot            = 0x2C, // another key not present on modern keyboards
    KeyInsert              = 0x2D, // insert
    KeyDelete              = 0x2E, // delete
    KeyHelp                = 0x2F, // help, sent when F1 is pressed
    KeyBar0                = 0x30, // number 0 in the top number bar
    KeyBar1                = 0x31, // number 1 in the top number bar
    KeyBar2                = 0x32, // number 2 in the top number bar
    KeyBar3                = 0x33, // number 3 in the top number bar
    KeyBar4                = 0x34, // number 4 in the top number bar
    KeyBar5                = 0x35, // number 5 in the top number bar
    KeyBar6                = 0x36, // number 6 in the top number bar
    KeyBar7                = 0x37, // number 7 in the top number bar
    KeyBar8                = 0x38, // number 8 in the top number bar
    KeyBar9                = 0x39, // number 9 in the top number bar

    // 0x3A to 0x40 is undefined

    KeyA                   = 0x41,
    KeyB, KeyC, KeyD, KeyE, KeyF, KeyG, KeyH, KeyI, KeyJ, KeyK, KeyL, KeyM, KeyN, KeyO, KeyP, KeyQ, KeyR, KeyS,
    KeyT, KeyU, KeyV, KeyW, KeyX, KeyY,
    KeyZ                   = 0x5A,
    KeyLWin                = 0x5B, // left Windows key
    KeyRWin                = 0x5C, // right Windows key
    KeyApps                = 0x5D, // applications key

    // 0x5E is reserved

    KeySleep               = 0x5F, // sleep, I am not sure if this is ever going to be used
    KeyPad0                = 0x60, // number 0 on the keypad
    KeyPad1                = 0x61, // number 1 on the keypad
    KeyPad2                = 0x62, // number 2 on the keypad
    KeyPad3                = 0x63, // number 3 on the keypad
    KeyPad4                = 0x64, // number 4 on the keypad
    KeyPad5                = 0x65, // number 5 on the keypad
    KeyPad6                = 0x66, // number 6 on the keypad
    KeyPad7                = 0x67, // number 7 on the keypad
    KeyPad8                = 0x68, // number 8 on the keypad
    KeyPad9                = 0x69, // number 9 on the keypad
    KeyMultiply            = 0x6A, // multiply key on the keypad
    KeyAdd                 = 0x6B, // add key on the keyboard
    KeySeparator           = 0x6C, // separator
    KeySubtract            = 0x6D, // subtract key on the keypad
    KeyDecimal             = 0x6E, // decimal, probably also on the keypad
    KeyDivide              = 0x6F, // divide key on the keypad
    KeyF1 = 0x70,
    KeyF2, KeyF3, KeyF4, KeyF5, KeyF6, KeyF7, KeyF8, KeyF9, KeyF10, KeyF11, KeyF12, KeyF13, KeyF14, KeyF15, KeyF16,
    KeyF17, KeyF18, KeyF19, KeyF20, KeyF21, KeyF22, KeyF23,
    F24 = 0x87,

    // 0x88 to 0x8F is reserved

    KeyNumLock             = 0x90, // literally why?
    KeyScroll              = 0x91, // scroll, not the one on the mouse

    // 0x97 to 0x9F is not assigned to anything

    KeyLShift              = 0xA0, // left shift
    KeyRShift              = 0xA1, // right shift
    KeyLControl            = 0xA2, // left control
    KeyRControl            = 0xA3, // right control
    KeyLAlt                = 0xA4, // left alt
    KeyRAlt                = 0xA5, // right alt
    KeyBrowserBack         = 0xA6, // browser back, yep ... no clue either
    KeyBrowserForward      = 0xA7, // browser forward
    KeyBrowserRefresh      = 0xA8, // browser refresh
    KeyBrowserStop         = 0xA9, // browser stop
    KeyBrowserSearch       = 0xAA, // browser search
    KeyBrowserFav          = 0xAB, // browser favorites
    KeyBrowserHome         = 0xAC, // browser home
    KeyVolumeMute          = 0xAD, // volume key, mute system sound
    KeyVolumeDown          = 0xAE, // volume key, make system sound quieter
    KeyVolumeUp            = 0xAF, // volume key, make system sound louder
    KeyMediaNext           = 0xB0, // media key, play next track
    KeyMediaPrev           = 0xB1, // media key, play previous track
    KeyMediaStop           = 0xB2, // media key, quit playback
    KeyMediaPlayPause      = 0xB3, // media key, continue playback or pause
    KeyLaunchMail          = 0xB4, // launch mail
    KeyMediaSelect         = 0xB5, // select media key
    KeyLaunch1             = 0xB6, // launch key 1, keyboard manufacturer specific
    KeyLaunch2             = 0xB7, // launch key 2, keyboard manufacturer specific

    // 0xB8 and 0xB9 are reserved

    KeyOem1                = 0xBA, // misc characters, varies by keyboard layout (on US-layout: ";" or ":")
    KeyPlus                = 0xBB, // the + key for any layout
    KeyComma               = 0xBC, // the , key for any layout
    KeyMinus               = 0xBD, // the - key for any layout
    KeyPeriod              = 0xBE, // the . key for any layout
    KeyOem2                = 0xBF, // misc characters, varies by keyboard layout (on US-layout: "/" or "?")
    KeyOem3                = 0xC0, // misc characters, varies by keyboard layout (on US-layout: "`" or "~")

    // 0xC1 to 0xDA is reserved

    KeyOem4                = 0xDB, // misc characters, varies by keyboard layout (on US-layout: "[" or "{")
    KeyOem5                = 0xDC, // misc characters, varies by keyboard layout (on US-layout: "\\" or "|")
    KeyOem6                = 0xDD, // misc characters, varies by keyboard layout (on US-layout: "]" or "}")
    KeyOem7                = 0xDE, // misc characters, varies by keyboard layout (on US-layout: "'" or """)
    KeyOem8                = 0xDF, // misc characters, varies by keyboard layout

    // 0xE0 is reserved and 0xE1 is OEM specific

    KeyOem102              = 0xE2, // misc characters, varies by keyboard layout (on US102-layout: "<" or ">", US-non-102: "\\" or "|")

    // 0xE23 and 0xE4 are OEM specific

    KeyProcessKey          = 0xE5, // ime process key
    KeyPacket              = 0xE7, // DO NOT USE, passed as the lower 32-bit for non-keyboard input methods.

    // 0xE6 and 0xE8 to F5 is either OEM specific or unused

    KeyAttn                = 0xF6, // attn key
    KeyCrSel               = 0xF7, // CrSel key
    KeyExSel               = 0xF8, // ExSel key
    KeyErEof               = 0xF9, // erase end-of-file key
    KeyPlay                = 0xFA, // playback
    KeyZoom                = 0xFB, // change zoom level
    KeyNoName              = 0xFC, // unused but still valid key
    KeyPa1                 = 0xFD, // pa1 key
    KeyClearOem            = 0xFE  // oem specific clear key
};

// Messagebox flags, use to specify the format and look of a message box
enum MessageBoxFlags : uint16_t
{
    // Block execution as long as the message box is open
    MbTaskModal                   = 0b1,

    // Icons
    MbIconWarning                 = 0b10,
    MbIconError                   = 0b100,
    MbIconInfo                    = 0b1000,
    MbIconQuestion                = 0b10000,

    // Buttons, 2 options
    MbButtonOk                    = 0b100000,
    MbButtonOkCancel              = 0b1000000,
    MbButtonYesNo                 = 0b10000000,
    MbButtonRetryCancel           = 0b100000000,

    // Buttons, 3 options
    MbButtonYesNoCancel           = 0b1000000000,
    MbButtonAbortRetryIgnore      = 0b100000000000,
    MbButtonCancelRetryContinue   = 0b1000000000000
};

// Message box return, indicates what button was pressed to close the message box
// The X-Button on the message box also gives back MrCancel
enum MessageBoxReturn : uint8_t
{
    MrAbort    = 1,
    MrCancel,
    MrContinue,
    MrIgnore,
    MrNo,
    MrOk,
    MrRetry,
    MrTryAgain,
    MrYes
};

// Window position relation
// Used for the WindowGetPosition functions
enum WindowPositionRelation : uint8_t
{
    // WindowGetXPos
    WpLeft     = 1,
    WpRight,

    // WindowGetYPos
    WpTop,
    WpBottom,

    // WindowGetPosition
    WpTopLeft,
    WpTopRight,
    WpBottomLeft,
    WpBottomRight
};

enum WindowVisibility : uint8_t
{
    WvHide, // hide
    WvNormal, // show and activate
    WvActivateMinimized, // minimize and activate
    WvActivateMaximised, // maximize and activate
    WvNormalNoActivate, // show and don't activate
    WvActivate, // activate
    WvMinimize, // minimize and deactivate
    WvMinimizeNoActivate, // the same as WvMinimize?
    WvActivateNoActivate, // XD
    WvRestore // restore from minimize and activate
};

enum ApplicationStyle : uint8_t
{
    AsNoIntegratedRenderer      = 0b1,
    AsNoCloseButton             = 0b10,
};

struct InitialisationData
{
    const char* pIconPath;
    const char* pCursorPath;
    uint8_t appStyle;
} __attribute__((aligned(32)));

struct WindowCreateData
{
    uint16_t width, height;
    int16_t xPos, yPos;
    const wchar_t* pName;

    std::vector<WndH> dependants;

    void (*pfOnClose)();
    bool (*fpOnCloseAttempt)();
} __attribute__((aligned(64)));

struct Rectangle
{
    int16_t bottom, top, left, right;
} __attribute__((aligned(8)));

struct MouseInfo
{
    int xPos, yPos;
    bool left, right, middle, x1, x2;
    WndH containingWindow;
} __attribute__((aligned(16)));

// Start the entire framework up, so it can be used.
void Initialise(const InitialisationData& initialisationData);

// Shutdown, cleanup.
void UnInitialise();

// Create a new window
WndH CreateWindowAsync(const f::WindowCreateData& windowCreateData);

WndH CreateWindowSync(const f::WindowCreateData& windowCreateData);

// Attempts to close a window
// This function will order OnWindowCloseAttempt to be called which means that nothing will happen if the request
// is refused
void CloseWindow(WndH handle);

// Close the window, don't care if it refuses
// OnWindowCloseAttempt will not be called
void CloseWindowForce(WndH handle);

// Close all windows
// This function will order OnWindowCloseAttempt to be called which means that nothing will happen to a window if
// it refuses
void CloseAllWindows();

// Close all windows, don't care if some windows refuse
// OnWindowCloseAttempt will not be called
void CloseAllWindowsForce();

void SetWindowVisibility(WndH handle, WindowVisibility visibility);

// Specify a function to be executed when a window is requested to be closed
// The function must return a boolean indicating whether the window should be closed
void OnWindowCloseAttempt(WndH handle, bool(*func)());

// Specify a function to be executed when a window is closed
void OnWindowClose(WndH handle, void(*func)());

// Returns the name of the window which matches the given handle
// This function can fail if the handle is not valid
wchar_t* GetWindowName(WndH handle);

// Returns the state of visibility of the window which matches the given handle
// This function always returns false if the handle is invalid
bool GetWindowVisibility(WndH handle);

// Returns the width of the window which matches the given handle
// This function will always return NULL if the handle is invalid
uint16_t GetWindowWidth(WndH handle);

// Returns the height of the window which matches the given handle
// This function will always return NULL if the handle is invalid
uint16_t GetWindowHeight(WndH handle);

// Returns the window size in form of a pair
// This function will always return NULL for both values if the handle is invalid
std::pair<uint16_t, uint16_t> GetWindowDimensions(WndH handle);

// This function can fail if the handle is invalid
bool GetWindowRectangle(WndH handle, Rectangle& wpr);

// Returns the number of open windows
int GetWindowCount();

// Changes the name of the specified window to the given name
// Can fail if the handle is invalid
void ChangeWindowName(WndH handle, const wchar_t* name);

// Return whether the specified window has keyboard focus or not
bool WindowHasFocus(WndH handle);

// Returns whether the passed handle corresponds to an existing window or not
bool IsValidHandle(WndH handle);

// Returns true as long as any window is open
bool Running();

// Pause the caller thread for the specified amount of ms
void Halt(int milliseconds);

// Creates a message box with the given information
// Can only have one owner
int MessageBox(WndH owner, const wchar_t* title, const wchar_t* msg, uint16_t flags);

// Check the key state for the specified key, return true if the key is pressed
bool IsKeyPressed(Key code);

// Check the key state for the specified key, return true if the key is pressed
// The function being sticky refers to the reset of the key state that is being checked. When the checked key is
// pressed, the function will reset the internal key state even while the physical button is still being pressed.
// Quite useful for toggle-like behaviour where the key is being held down for multiple calls
bool IsKeyPressedOnce(Key code);

// Returns true if the specified key is released.
// This function must be executed in a loop since it stores the key state at the last call.
bool IsKeyReleased(Key code);

// Returns true if any key is pressed.
bool IsAnyKeyPressed();

// Enables / disables the text input field.
void SetTextInputState(bool state, bool clear = true);

// Returns a pointer to the text input field.
const wchar_t* GetTextInput();

// Clears the text input field out to all 0's.
void ClearTextInput();

// Returns whether the text input field is enabled or not.
bool GetTextInputState();

// Returns a struct containing all information about the state of the mouse.
// Useful when checking multiple factors of the mouse since you won't have to make multiple function calls.
MouseInfo GetMouseInfo();

// Returns the mouse position on the x coordinate.
int GetMouseX();

// Returns the mouse position on the y coordinate.
int GetMouseY();

// Returns true when the right mouse button is pressed and your application has focus.
bool GetMouseLeftButton();

bool GetMouseLeftButtonOnce();

// Returns true when the right mouse button is pressed and your application has focus.
bool GetMouseRightButton();

bool GetMouseRightButtonOnce();

// Returns true when the middle mouse button is pressed and your application has focus.
bool GetMouseMiddleButton();

bool GetMouseMiddleButtonOnce();

// Returns true when the first extra mouse button is pressed and your application has focus.
bool GetMouseX1Button();

bool GetMouseX1ButtonOnce();

// Returns true when the second extra mouse button is pressed and your application has focus.
bool GetMouseX2Button();

bool GetMouseX2ButtonOnce();

// Returns the number of steps the mouse wheel was scrolled compared to the last time this function was called.
// A negative value means a scroll down, a positive one means a scroll up. Null means the wheel has not moved.
// Must be executed in a loop.
int GetMouseWheelDelta();

// Returns true if the mouse is hovering over the specified window. It does not matter if the window has focus or not
// Always returns false if the handle is invalid.
bool WindowContainsMouse(WndH handle);

// Returns a handle to the window that currently contains the mouse. This is independent of focus.
// If none of your window contains the mouse it will return null and set an error.
// You may choose to ignore the error if intended as it is insignificant.
WndH GetMouseContainerWindow();

// Returns a handle to the window that currently has focus
// Returns NULL none of the owned windows has focus
WndH GetWindowWithFocus();

// TODO: file to byte array
char* FileToByteArray(size_t* bytes);

} // end namespace f
