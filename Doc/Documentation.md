# Internal Documentation
This is the internal documentation used to document all framework related stuff. Everything in here is not
exposed to the framework-user.

### Window Data
Content:
- Window* (points to data created in the main function of the user)
- Thread* (thread which executes the message pump for the specific window, allocated on the heap)
- HWND (window handle for Win32 usage)

### Internal Error Handling
When an internal error occours it will be handled by the framework itself. If there is no way to handle it, the
application must quit.

Internal Error sources are:
- inner framework operations
- Win32 API
- Vulkan

Depending on the build type the error will be created differently. For Debug, a message box will be created,
displaying debug level information. When building release, the same information will be writen to a file
called `Last_Error.txt`.

For external errors caused by missuse of the framework, see the coresponding section in the user manual.