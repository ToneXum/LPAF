# tsd::OnWindowCloseAttempt
Used to specify or change the function to be called when a window is requested to be closed.

### Definition
```C++
void OnWindowCloseAttempt(
    WndH handle, 
    bool(*func)(void)
    );
```

### Parameters
`WndH handle`:
A handle to the window who's function pointer should be changed.

`bool (*func)(void)`:
A function pointer to a function that will be called when a window is requested to be closed. A close request happens when the X button (non-client region or taskbar) or `Alt + F4` is pressed. The function must a return a boolean value indicating whether the window should actually be closed. If it returns `true` on a call, the visible surface will be destroyed and `OnClose` will be called. If it returns `false`, nothing will happen and the window stays open.

### Remarks
This function is being executed by the thread that manages the window data. It must therefore not cause any unhandled exceptions to be thrown. Otherwise the entire application will break.
