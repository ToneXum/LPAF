# User Documentation
GGFW is a perfomant and light-weight application framework used for development on Windows systems.

## 1. Getting started
To use the framework, you have to link the library as well as including the header(s) in your project.

This is a static library which means it has to be linked at compile time. (I personaly see no reason to use a 
dynamic library so here you go.)

### Setup for Visual Studio
Including the header:
- go to the project properties
- in the `C++` section, open the `General` page
- add the directory of the header(s) to `Aditional Include Directories`
- make sure this change applies to all configurations

Of course you can also put the header into your project directory if you want.

Linking the library:
- go to the project propertiers
- in the `Linker` section, open the `Input` page
- add the path to `Framework.lib` into `Aditional Dependencies`
- make sure this change applies to all configurations

If you also want to get rid of the command prompt when running the program in the debugger you can change the
subsystem options:
- go to the project properties
- in the `Linker` section, open the `system` page
- select `Windows`

Now, your application is technically a Win32 API program which requires its own entry point. To use the normal 
`main` entry point you need to add `/ENTRY:mainCRTStartup` to the linker options in the `Additional Options`
section of the `Command Line` page. Otherwise your entrypoint would look like this:

`int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)`

**If you use a different IDE, look up how you can replicate the same steps in your environment**

## 2. Usage
The usage of this framwork is kept as simple as possible without comprimising on performance.

### Basic Program
To start using the framework you need to initialise it and make a window. After that comes your logic loop 
followed by a clean-up. For this framework, the most basic setup should look like this:

```C++
// This example uses the main entry point for convenience
int main()
{
	// This starts the framwork, only after this call can any part of it be used
	tsd::Initialise();

	// Create a window
	// If `tsd::CreateWindow()` succedes, you get a handle which with you can identify your window
	const char* windowName = "Boring Box";
	short handle = tsd::CreateWindow(windowName, 1000, 500);

	// `tsd::Running` will return a boolean indicating whether any window is open or 
	while (tsd::Running())
	{
		// Logic loop goes here
	}

	// Clean up and stop the framework
	tsd::Unitialise();

	return 0;
}
```

## 3. Error handling
The framework features error handling very similar to the Win32 API. This maximises the flexibility about how 
you handle the actual errors.

When an error occours, the callee will set an internal variable which indicates the code of the last error. You
can retrieve that code by calling `GetLastError()`. To get a readable message out of said error code you can
pass it to `GetErrorInformation(int)` and from there you can handle the error in anyway you like. I personally
recommend throwing an exception to conform to the way internal errors are handled.

Note that the error is only valid when the callee return `null` or `null-pointer` and also be sure that the
underlying function *can* set an error code in the first place.

#### Internal Errors
Errors however can not only be caused by the user. Sometimes errors can occour within the the framework itself.
In that case, the framework will automatically open a message box with debugging information and quit the entire
application. After the message box is closed it will throw an exception on the main thread. To properly clean up
everything with stack unwinding it is advised to put everything into a try-catch statement.

```C++
int main()
{
	try
	{
		// All your stuff goes here

		return 0;
	}
	catch (const std::exception&) // The framework only throws std::exception
	{
		// Still perform clean-up to avoid memory leaks
		tsd::Unitialise();
		return -1;
	}
}
```

### Specifics
Now that you have a basic program running you can get into more specific topics such as customisation, interaction
and many more things.

These are the more specific topics currently available
- [Window Customisation](/Usage_Customisation.md)
