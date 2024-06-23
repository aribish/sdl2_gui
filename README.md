# sdl2_gui
No BS GUI library for SDL2

## How to Install
Run `make dynamic` or `make static` to generate a dynamic or static library respectively. Move the header and library files to their designated directories on your operating system.


## Usage
You are expected to initialize an SDL_Window and SDL_Renderer. This library will initialize and terminate SDL_ttf and SDL_images, so you will need to link
these libraries when compiling your project.

## Updates
### 2024-06-22
I'm making this to use with my Odin synthesizer, so I will be making bindings for that soon, as well as some documentation and important notes. I still need
to add proper error handling, but as long you don't try to break everything, you probably won't (I hope so anyways).
