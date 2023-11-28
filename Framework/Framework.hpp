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

    class Mouse
    {
        // todo
    };

    class Keyboard
    {
        // idk man
    };

    // A window class to represent an open window
    class Window
    {
    public:
        Window(const char* name, int width, int height);
        Window(const char* name, int width, int height, int xPos, int yPos);
        ~Window();

        Window(const Window&) = delete;

        void ChangeName(const char* newName);
        void ChangeVisibility(bool state);
        void Minimize(bool state);
        void Resize(int width, int height);
        void Reposition(int xPos, int yPos);

        unsigned int GetId();
        char* GetName();
        bool GetVisibility();
        int GetWidth();
        int GetHeight();

    private:
        unsigned int id;
        char* name;
        bool isVisible;
        //long long hWnd; in memory of a not so nice hack
        int xPos, yPos, width, height;
    };
    
    // Returns the number of open windows
    int GetWindowCount();

    // Returns a pointer to the instance of a window which matches the specified id
    // This function can fail when there is no window with the specified id, if so, it returns nullptr
    Window* GetWindow(int id);

    // Returns a pointer to the instance of a window which matches the specified name
    // This function can fail when there is no window with the specified name, if so, it returns nullptr
    Window* GetWindow(const char* name);

    // Returns true as long as any window is open
    inline bool Running();

    // Pause the caller thread for the specified ammout of ms
    void Halt(int ms);
}
