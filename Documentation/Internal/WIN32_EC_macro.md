# WIN32_EC
Macro used to check Win32 API calls for errors.

## Definition
Defined in [Internals.hpp](../../Framework/Internals.hpp).
```C++
#define WIN32_EC(expr, suc) { if (((expr) && (suc == 0)) || (!(expr) && (suc != 0))) \
{ i::CreateWin32Error(__LINE__, GetLastError(), __func__); } }
```

## Parameters
`expr`: <br>
The expression that is supposed to be evaluated.

`suc`: <br>
The success specifier. If it is `0`, it will be assumed that `expr` has succeeded if it evaluates to `0`. In that case,
the error handling will be triggered when `epxr` evaluates to `not 0`. When `suc` is not `0` (commonly `1`), the error 
handling will be triggered when `expr` evaluates to `0`.

## See also
To save the value that `expr` evaluates to you can use [WIN32_EC_RET](WIN32_EC_RET_macro.md).
