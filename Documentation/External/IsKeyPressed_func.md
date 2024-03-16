# f::IsKeyPressed
Utility function that is used for determining the state of a certain key on the keyboard.

## Definition
```C++
bool IsKeyPressed(
        [in] f::Key code
);
```

## Parameters
`f::Key code`: <br>
The keycode of the key that is supposed to be checked. For available keycodes you may refer to [f::Key](Key_enum.md). 

## Return value
`IsKeyPressed` will return true if the specified key is pressed, false if otherwise.
