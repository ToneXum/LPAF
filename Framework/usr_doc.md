# User Documentation
GGFW is a perfomant and light-weight application framework used for development on Windows systems.

### 1. Getting started
To use the framework, you have to like the library containing all the symbols as well as including the header in
your project.

This is a static library which means it has to be linked at compile time. (I personaly see no reason to use a 
dynamic library so here you go.)

#### Setup for Visual Studio
Including the header:
- go to the project properties
- in the `C++` section, open the `General` page
- add the directory of the header to `Aditional Include Directories`
- make sure this change applies to all configurations

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
`main` entry point you need to add `/ENTRY:mainCRTStartup` to the linker options

**If you use a different IDE, look up how you can replicate the same steps in your environment**