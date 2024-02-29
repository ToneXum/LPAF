# tsd::OnWindowClose
Used to specify or change the function to be called when a window is closed.

### Definition
```C++
void OnWindowClose(
    WndH handle, 
    void(*func)(void)
    );
```

### Parameters
`WndH handle`:
A handle to the window who's function pointer should be changed.

`void(*func)(void)`:
A function pointer that will be called when the specified window is closed.

### Remarks
This function is being executed by the thread that manages the window data. It must therefore not cause any unhandled exceptions to be thrown. Otherwise the entire application will break.
