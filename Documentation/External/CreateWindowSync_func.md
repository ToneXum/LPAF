# f::CreateWindowSync
Requests the creation of a new window on the desktop. You may only call this function after calling 
[f::Initialise](Initialise_func). This function does guarantee that the window will be created after it returns. If 
you don't care about that, you may use [f::CreateWindowAsync](CreateWindowAsync_func) which does not wait and is
therefore incredibly cheap.

### Definition
```C++
WndH f::CreateWindowSync(
    [in] const WindowCreateData& windowCreateData
    );
```

### Parameters
`const WindowCreateData& windowCreateData`: <br>
A reference to a structure that contains all the information needed to describe the window that is supposed to be 
opened. See [WindowCreateData](WindowCreateData_type.md) for more.

### Return value
`CreateWindowSync` returns a `WndH` (basically just a number) which is a handle that the user can use to identify the
window. When the function fails for whatever reason, a messages is printed to `stdout` and `CreateWindowSync`
returns 0.

### Remarks
This function requests a window to be opened. While the actual process happens asynchronously, this function does wait
for the window to be created before returning. This is useful when creating a window and immediately (within a few
milliseconds) starting to working on it. When this does not matter, you might want to use 
[f::CreateWindowAsync](CreateWindowAsync_func) since it does not wait. Nevertheless, LPAFs error handling for the
utility functions is good enough to not cause issues when interacting with a window that is still being created (aka: 
does not exist... yet).
