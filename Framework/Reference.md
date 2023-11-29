# Reference
This is the symbol reference. Here you can find information about each individual function, its parameters
and what each of them does.

#### tsd::Initialise( no args )
Initiales the framework so it can be used. When this function is not called and you still call other functions
of the framework it can cause everything from undefined behavior to an unclosable process.

#### tsd::Uninitialise( no args )
Cleans up data on the heap and ends threads used for the framework. If not called, memory will be leaked.

#### Window Class
