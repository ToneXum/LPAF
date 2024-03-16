# f::IsKeyReleased
A utility function used to check if a key was released between calls.

## Definition
```C++
bool IsKeyReleased(
        [in] f::Key code
);
```

## Parameters
`f::Key code`: <br>
The keycode of the key that is supposed to be checked. For available keycodes you may refer to [f::Key](Key_enum.md).

## Return value
This function returns true **once** if the key was released **between** function calls. Otherwise, it will always return
false.

## Remarks
This function stores the last key state in static storage duration. Therefore, the logic only works when this function 
is executed in a loop. This works very similarly to [IsKeyPressedOnce](IsKeyPressedOnce_func.md) with a one call delay.
