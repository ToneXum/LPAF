## WND_H
`WND_H` is a datatype that acts as a handle to the windows that the user creates.

### Definition
```C++
typedef short WND_H;
```

### Remarks
Because it is defined as a `short`, the handle can only reach a value of `SHRT_MAX` which is 32767. What happens when it overflows depends on the architecture. In most cases it would simply wrap which means that the handles will continue at -32768. The error handling in [CreateWindow](tsd_CreateWindow_function.md) prevents that however, returning 0 instead and printing a message to `stdout` indicating what just happened. One should technically not even be able to reach that point because who would open 32767 windows in one runtime?