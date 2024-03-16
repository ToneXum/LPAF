# f::ApplicationStyle
An enumeration of flags, used to modify the behaviour of LPAF. Used in the `appStyle` member of 
[InitialisationData](InitialisationData_struct.md).

## Definition
```C++
enum ApplicationStyle : uint8_t
{
    AsNoIntegratedRenderer      = 0b1,
    AsNoCloseButton             = 0b10,
};
```

## Members
`AsNoIntegratedRenderer`: <br>
Disables the integrated renderer of LPAF. It won't be started and won't cause any additional overhead when managing the
application state.

`AsNoCloseButton`: <br>
Disables the close button on the non-client region of all windows. On Microsoft Windows, the window can still be closed 
using other methods, like the close button in the task-bar preview. To create windows that can not be closed without
killing the entire process, see [OnWindowCloseAttempt](OnWindowCloseAttempt_func.md).
