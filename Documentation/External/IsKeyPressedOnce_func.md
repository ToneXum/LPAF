# f::IsKeyPressedOnce
Just like [IsKeyPressed](IsKeyPressed_func.md), this function checks the state for the specified key on the keyboard.
However, upon a call this function will reset the state of the specified key.

## Definition
```C++
bool IsKeyPressedOnce(
        [in] f::Key code
);
```

## Parameters
`f::Key code`: <br>
The keycode of the key that is supposed to be checked. For available keycodes you may refer to [f::Key](Key_enum.md).

## Return value
This function will return true if the specified key is pressed the first time this function was called. It will return
false if the key was not pressed **OR** if the function already checked the specified key for the time it was pressed.

## Remarks
This function is especially useful for situations where you only want to have behaviour execute once, as 
[IsKeyPressed](IsKeyPressed_func.md) returns true as long as the key is down.
