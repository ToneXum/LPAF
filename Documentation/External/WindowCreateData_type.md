# f::WindowCreateData
`WindowCreateData` is a structure type for data that you pass to [f::CreateWindowAsync](CreateWindowAsync_func).

## Definition
```C++
struct WindowCreateData
{
    [in] std::vector<WndH> dependants;
    
    [in] const wchar_t* pName;

    [in] void (*pfOnClose)();
    [in] bool (*fpOnCloseAttempt)();

    [in] uint16_t width, height;
    [in] int16_t xPos, yPos;
};
```

## Members
`std::vector<WndH> dependants`: <br>
A vector of window handles. When the window is closed, all windows matching the handles will also be closed. This 
mechanism can be evaded by simply refusing in the underlying `CloseWindowAttempt` function. For more information see the
two sections about the function pointers below.

`const wchar_t* pName`: <br>
The name of the window that is displayed on informational surfaces across the operating system. On Microsoft Windows 
that would be the non-client region of the window, the TaskManager, the Taskbar and other services that pull names of 
running applications. This pointer must point to a valid, null-terminated wide string.

`void (*OnClose)()`: <br>
A function pointer that will be called when the window is closed. Leave this `nullptr` to not use this feature. If you 
want to specify a function or change it after the window was created use [f::OnWindowClose](OnWindowClose_func).

`bool (*OnCloseAttempt)()`: <br>
A function pointer that will be called when the window is requested to be closed. A close request happens when the X 
button (non-client region or taskbar) or `Alt + F4` is pressed. The function must a return a boolean value indicating 
whether the window should actually be closed. If it returns `true` on a call, the visible surface will be destroyed and 

`OnClose` will be called. If it returns `false`, nothing will happen and the window stays open. If you want to specify a
function or change it after the window was created use [f::OnWindowCloseAttempt](OnWindowCloseAttempt_func).

`uint16_t width, height`: <br>
The width and height of the window in pixels. If one of these values is greater than the size of the screen, the window 
will be cropped to fit. Because the "origin point" of the window is the upper left corner, the window expands to the 
right from the "origin point" with more width and expands downwards with more height.

`int16_t xPos, yPos`: <br>
The x and y position of the upper left corner of the window. Leave them `0` to let the operating system decide on where 
to place the window. That way the operating system will also remember where the window was closed and will place the 
window at the old position when the app is reopened.
