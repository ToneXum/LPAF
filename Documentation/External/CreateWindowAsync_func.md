# f::CreateWindowAsync
Requests the creation of a new window on the desktop. You may only call this function after calling [f::Initialise](Initialise_func).
This function does not guarantee that the window will be created after this function returns. If this is a problem, see 
[CreateWindowSync](CreateWindowSync_func)

### Definition
```C++
WndH f::CreateWindowAsync(
    [in] const WindowCreateData& windowCreateData
    );
```

### Parameters
`const WindowCreateData& wndCrtDt`: <br>
A reference to a structure that contains all the information needed to describe the window that is supposed to be 
opened. See [WindowCreateData](WindowCreateData_type.md) for more.

### Return value
`CreateWindowAsync` returns a `WndH` (basically just a number) which is a handle that the user can use to identify the 
window. When the function fails for whatever reason, a messages is printed to `stdout` and `CreateWindowAsync` 
returns 0.

### Remarks
This function only requests a window to be opened. The actual process happens asynchronously which means it is not 
guaranteed that the window will be open after the function returns. This can save a pretty good amount of time when more
expensive work is being done in the meantime.
