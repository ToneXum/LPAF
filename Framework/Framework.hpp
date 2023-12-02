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
}
