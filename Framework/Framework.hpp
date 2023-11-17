#pragma once

namespace tsd // tonexum software division
{
    // Start the entirety of this framework up so it can be used.
    void Initialise(void);

    // commit self delete :)
    void Uninitialise(void);

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

        void ChangeName(const char* newName);
        void ChangeVisibility(bool state);
        void Minimize(bool state);
        void Resize(int width, int height);
        void Reposition(int xPos, int yPos);

        unsigned int GetId(void);
        char* GetName(void);
        bool GetVisibility(void);
        int GetWidth(void);
        int GetHeight(void);

    private:
        unsigned int id;
        char* name;
        bool isVisible;
        //long long hWnd; in memory of a not so nice hack
        int xPos, yPos, width, height;
    };
    
    // Returns the number of open windows
    int GetWindowCount(void);

    // Returns an instance of a window. Identified by Id
    Window* GetWindow(int id);

    // Returns an instance of a window. Identified by name
    Window* GetWindow(const char* name);

    // Returns the progam running state, false means you should exit
    bool Running();

    // Sleep for specified ammout of ms
    void Halt(int ms);
}
