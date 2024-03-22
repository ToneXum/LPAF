# f::FrameworkInitData
A structure used to specify modification in behaviour of your LPAF application.

## Definition
```C++
struct FrameworkInitData
{
    [in] int iconId, cursorId;
    [in] uint8_t appStyle;
};
```

## Members
`int iconId`: <br>
**DO NOT USE, REWORK OF THIS FEATURE IS IN PROGRESS**

`int cursorID`: <br>
**DO NOT USE, REWORK OF THIS FEATURE IS IN PROGRESS**

`uint8_t appStyle`: <br>
The application style. This is a combination of flags from the [ApplicationStyle](ApplicationStyle_enum.md) enum. 
Bitmask these flags together to create the targeted behaviour. 
