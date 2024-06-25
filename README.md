# sdl2_gui
No BS GUI library for SDL2

## Disclaimer
There are much more powerful and efficient GUI libraries that use OpenGL that integrate with SDL2. This library is primarily for small, quick projects
where you don't necessarily want to add all the boilerplate code required to use these larger libraries.

## Installation
Run `make dynamic` or `make static` to generate a dynamic or static library respectively. Move the header and library files to their designated directories on your operating system.

The "sdl2_gui" directory contains bindings for this library in Odin. After installing the dynamic library file, you can move this directory to the "vendor" directory
located at your Odin install path.

## Example
You can run `make example` or just `make` in the project directory to generate an example executable. There isn't really anything actually interesting
being displayed, but the source code kind of shows off how you might integrate this library into a personal project.

## Updates

### 2024-06-25
I completely rewrote the event system and added a couple of new event-related utility functions. I also fixed a few bugs that came up and made bindings for Odin.
I think this is pretty much done for now.

### 2024-06-22
I'm making this to use with my Odin synthesizer, so I will be making bindings for that soon, as well as some documentation and important notes. I still need
to add proper error handling, but as long you don't try to break everything, you probably won't (I hope so anyways).
