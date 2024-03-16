# f::OnWindowClose
Used to specify or change the function to be called when a window is closed.

## Definition
```C++
void OnWindowClose(
    [in] WndH handle, 
    [in] void(*func)(void)
    );
```

## Parameters
`WndH handle`: <br>
A handle to the window whose function pointer should be changed.

`void(*func)(void)`: <br>
A function pointer that will be called when the specified window is closed.

## Remarks
This function is being executed by the thread that manages the window data. It must therefore not cause any unhandled 
exceptions to be thrown. The entire application will break otherwise.
