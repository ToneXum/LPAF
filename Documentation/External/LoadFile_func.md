# f::LoadFile
Loads a file into memory

## Definition
```C++
void* LoadFile(
        [in] const char* file, 
        [out] size_t& bytes
);
```

## Parameters
`const char* file`: <br>
A filename or a path to a file that is supposed to be loaded into memory. If the file specified is read-protected or 
does not exist, the function will fail.

`size_t& bytes`: <br>
The specified data field will be modified to reflect the amount of bytes the file took up while loading (i.e. the size 
of the file in bytes). If the function fails this value will not be modified

## Return value
This function will return a pointer that points to the first byte of the file in memory. If the function fails, it will
return `nullptr`.

## Remarks
When loaded, the file is allocated on the heap. This means that the data has to be freed when it becomes unused. 
Disregarding the return value will therefore lead to memory leaks
