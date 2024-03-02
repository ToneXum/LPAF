# f::UnInitialise
Used to clean up the data the used by the framework. Only after a call to this function should the application quit.

### Definition
```C++
void UnInitialise();
```

### Remarks
Do not call this function while you are not sure that all windows are closed. If you do so otherwise, you will get a 
framework-internal error which will kill applications process. Do also not call this function multiple times nor 
before calling [f::Initialise](Initialise_function.md). It will result in use-after-free exceptions.