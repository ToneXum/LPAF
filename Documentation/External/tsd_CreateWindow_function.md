# tsd::CreateWindow
The `CreateWindow` function can be used to open a new window on the desktop. You may only call this function after calling [tsd::Initialize](tsd_Initialize_function.md)

### Definition
```C++
WND_H tsd::CreateWindow(
    const WindowCreateData& wndCrtDt
    );
```

### Parameters
`const WindowCreateData& wndCrtDt`:
A reference to a structure that contains all the information needed to describe the window that is supposed to be opened. See [WindowCreateData](tsd_WindowCreateData_type.md) for more.

### Return value
`CreateWindow` returns a `WND_H` (basically just a number) which is a handle that the user can use to identify the window. When the function fails for whatever reason, a messages is printed to `stdout` and `CreateWindow` returns 0.

### Remarks
Because the windows are managed by another thread, the first call to `CreateWindow` will take about 20 times longer than calls to it after. This is because `CreateWindow` needs to wait for the thread to start its execution. How long this actually takes is up to the task scheduler of the operating system. When the thread is already started, the actual window creation is fully asynchronous. For reference: On my machine first call takes about 40ms, after that its only 2ms.
