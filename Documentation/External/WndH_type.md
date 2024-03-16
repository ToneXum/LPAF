# f::WndH
`WndH` is a datatype that acts as a handle to the windows that the user creates.

## Definition
```C++
using WndH = uint16_t;
```

## Remarks
Because it is defined as an unsigned short, the handle can only reach a value of 65'536. What happens when it overflows
depends on the architecture. In most cases it would simply wrap which means that the handles will continue at 0. The
error handling in the window creation functions ([f::CreateWindowAsync](CreateWindowAsync_func) and 
[f::CreateWindowSync](CreateWindowSync_func.md)) prevents the overflow however, then returning 0 anyway to indicate that
an error occurred and printing a message to `stdout` indicating what just happened. One should technically not even be 
able to reach that point because who would open 65'536 windows in one runtime?