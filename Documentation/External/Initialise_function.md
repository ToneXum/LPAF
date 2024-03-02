# f::Initialise
Used to start up the framework. Only after a call to this function are you able to use the main features of LPAF.

### Definition
```C++
void Initialise(
    int icon, 
    int cursor
    );
```

### Parameters
`int icon`:
An identifier for an icon resource. To use the standard icon from the operating system, pass `0`. For more information 
regarding application customization, see the [Customization Guide](../Guides/Class_Customization.md).

`int cursor`:
An identifier for a cursor resource. To use the standard cursor from the operating system, pass `0`. For more 
information regarding application customization, see the [Customization Guide](../Guides/Class_Customization.md).
