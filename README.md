# gaiasim
> A fantasy-realism life simulation game using [raylib](https://github.com/raysan5/raylib) for desktop, mobile, and web.

## File Structure
- `./android/`: Android project, bundles native C/C++ code into an .apk file.
- `./build/`: Build directories for all platforms. This is where executables are output to.
- `./external_libs/`: External libraries, these are added as Git submodules.
- `./src/`: The main project C/C++ source code, shared between all platforms.
- `./resources/`: Static resources such as images, sounds, text files, etc that are loaded in by main project code.