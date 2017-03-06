# Framework

The goal of this framework is to offer basic functionality so the user can focus on creating an OpenGL application. 

## Features

- Creates a context
- Executes necessary initializations
- Runs the render loop and calls the virtual functions which a user shall implement
- Offers helper-classes (such as image and model for asset loading)

New helper-classes shall be implemented when needed. Nevertheless the user should not be forced to use them.