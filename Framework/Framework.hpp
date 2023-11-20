#pragma once

// For anyone recieving a copy of this file:
// Modify at your own risk ;)

namespace tsd // tonexum software division
{
    // Start the entirety of this framework up so it can be used.
    void Initialise();

    // commit self delete :)
    void Uninitialise();

    // If an error occours, check the code with this function
    // Beware only check the return value of functions that actually set the 
    int GetLastError();

    // some information about an error
    struct Error
    {
        bool isValidError; // use for controll 
        const char* msg; // message or meaning
    };

    // translates the error by code into a readable message
    const char* GetErrorInformation(int code);

    class Mouse
    {
        // todo
    };

    class Keyboard
    {
        // todo
    };

    // A window class to represent an open window
    class Window
    {
    public:
        Window(const char* name, int width, int height);
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

    // Returns an instance of a window. Identified by Id
    // This function can fail when there is no window with the specified id
    Window* GetWindow(int id);

    // Returns an instance of a window. Identified by name
    // This function can fail when there is no window with the specified name
    Window* GetWindow(const char* name);

    // Returns the progam running state, false means you should exit
    bool Running();

    // Sleep for specified ammout of ms
    void Halt(int ms);
}
