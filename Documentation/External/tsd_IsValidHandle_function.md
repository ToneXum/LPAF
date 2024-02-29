# tsd::IsValidHandle
Used to test if a handle is valid.

### Definition
```C++
bool IsValidHandle(
    WndH handle
    );
```

### Parameters
`WndH handle`:
The handle to be tested.

### Return value
If the specified handle belongs to a window that is currently open, `true` will be returned. `false` if otherwise.