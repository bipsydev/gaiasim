# gaiasim
> A fantasy-realism life simulation game using [raylib](https://github.com/raysan5/raylib) for desktop, mobile, and web.

## File Structure
- `./android/`: Android project, bundles native C/C++ code into an .apk file.
- `./assets/`: Static resources such as images, sounds, text files, etc that are loaded in by main project code.
- `./build/`: CMake build directory, mostly just intermediate build files.
- `./build_output/`: Contains the final built executable or apk file, in the corresponding platform subdirectory.
- `./external_libs/`: External libraries, these are added as Git submodules.
- `./include/`: The main project C/C++ header files.
- `./src/`: The main project C/C++ source code, shared between all platforms.

## Setup Guides from raylib
- [Linux](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux) setup guide
- [Android on Linux](https://github.com/raysan5/raylib/wiki/Working-for-Android-(on-Linux)) build/compilation guide
