# f::OnWindowCloseAttempt
Used to specify or change the function to be called when a window is requested to be closed.

## Definition
```C++
void OnWindowCloseAttempt(
    [in] WndH handle, 
    [in] bool(*func)(void)
    );
```

## Parameters
`WndH handle`: <br>
A handle to the window whose function pointer should be changed.

`bool (*func)()`: <br>
A function pointer to a function that will be called when a window is requested to be closed. A close request happens 
when the X button (non-client region or taskbar) or `Alt + F4` is pressed. The function must a return a boolean value 
indicating whether the window should actually be closed. If it returns `true` on a call, the visible surface will be 
destroyed and `OnClose` will be called. If it returns `false`, nothing will happen and the window stays open.

## Remarks
The specified function is being executed by the thread that manages the window data. It must therefore not cause any 
unhandled exceptions to be thrown. The entire application will break otherwise.
