#pragma once

// For anyone recieving a copy of this file:
// Modify at your own risk ;)

namespace tsd // tonexum software division
{
    // Start the entirety of this framework up so it can be used.
    void Initialise();

    // commit self delete :)
    void Uninitialise();

    // If an error occours, the callee returns null. Read the error code with this function
    // Beware only check the return value of functions that actually set errors
    int GetLastError();

    // Translates the error by code into a readable message
    const char* GetErrorInformation(int code);

    // Create a new window 
    short CreateWindow(const char* name, int width, int height, int xPos = 0, int yPos = 0);

    // Returns the name of the window which matches the given handle
    // This function can fail if the handle is not valid
    char* GetWindowName(short id);

    // Returns the state of visibility of the window which matches the given handle
    // This function can fail if the handle is not valid
    bool GetWindowVisibility(short id);

    // Returns the width of the window which matches the given handle
    // This function can fail if the handle is not valid
    int GetWindowWidth(short id);

    // Returns the height of the window which matches the given handle
    // This function can fail if the handle is not valid
    int GetWindowHeight(short id);

    // Returns the number of open windows
    int GetWindowCount();

    // Returns true as long as any window is open
    bool Running();

    // Pause the caller thread for the specified ammout of ms
    void Halt(int ms);

    // Messagebox flags
    // biggest possible size is 3 bytes, passed as 4 byte int
    enum MBF
    {
        // Let the exexting thread sleep as long as the box is open
        TASKMODAL   = 0b1,

        // Icons
        ICON_WARNING    = 0b10,
        ICON_ERROR      = 0b100,
        ICON_INFO       = 0b1000,
        ICON_QUESTION   = 0b10000,

        // Buttons, 2 options
        BUTTON_OK           = 0b100000,
        BUTTON_OK_CANCEL    = 0b1000000,
        BUTTON_YES_NO       = 0b10000000,
        BUTTON_RETRY_CANEL  = 0b100000000,

        // Buttons, 3 options
        BUTTON_YES_NO_CANCEL        = 0b1000000000,
        BUTTON_ABORT_RETRY_IGNORE   = 0b100000000000,
        BUTTON_CANCEL_RETRY_CONTINUE= 0b1000000000000
    };

    // Message box return
    // This is the meaning of the return value gotten from CreateMessageBox
    // Basically just the button that was pressed
    enum MBR
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

    // Creates a message box with the given information
    // Can only have one owner
    MBR CreateMessageBox(short owner, const char* title, const char* msg, int flags);
}
