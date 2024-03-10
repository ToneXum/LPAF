# f::Key
A set of virtual key identifiers. Used for various utility functions regarding the state of the keyboard.

### Definition
Enumeration prefix: `Key`
```C++
enum Key : uint8_t
{
    KeyLButton             = 0x01,
    KeyRButton             = 0x02,
    
    ...
    
    KeyPa1                 = 0xFD,
    KeyClearOem            = 0xFE 
};
```

### Usage
This enumeration is passed as a parameter for [IsKeyPressed](IsKeyPressed_func.md), 
[IsKeyPressedOnce](IsKeyPressedOnce_func.md) and [IsKeyReleased](IsKeyReleased_func.md).

### Remarks
This enumeration also includes system keys like the super-key, media-keys and a lot more (on Windows systems, even the 
power-off button on the front of the computer chassis can be used). It is not advised that you use these keys. Same goes
for old keys that are not usually present on modern keyboards as well as physical translation methods / transcription 
keys.
