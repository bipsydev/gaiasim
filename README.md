# gaiasim
> A fantasy-realism life simulation game using [SDL3](https://github.com/libsdl-org/SDL) for desktop, mobile, and web.

## File Structure
- `./android-project/`: Android project, bundles native C/C++ code into an .apk file and sets some Android specific settings. This was forked from the SDL provided template at `external_libs/SDL/android-project`.
- `./assets/`: Static resources such as images, sounds, text files, etc that are loaded in by main project code.
- `./build/`: CMake build directory, mostly just intermediate build files.
- `./build_output/`: Contains the final built executable or apk file, in the corresponding platform subdirectory.
- `./external_libs/`: External libraries, these are added as Git submodules.
- `./include/`: The main project C/C++ header files.
- `./src/`: The main project C/C++ source code, shared between all platforms.

### Scripts
- `./android_init.sh/`: Initializes the android-project directory from the SDL template. This relies on our project's `android_build.gradle` file replacing the template's `app/build.gradle`.
- `./android_debug.sh/`: runs `gradlew assembleDebug` to create the APK through CMake, then attempts to install it to a connected device via `adb` and display `adb logcat` output related to the app for debugging.

## SDL3 Guides and Examples
- [SDL with CMake](https://github.com/libsdl-org/SDL/blob/main/docs/INTRO-cmake.md)
- [Android build guide](https://github.com/libsdl-org/SDL/blob/main/docs/README-android.md)
- [Ravbug/sdl3-sample](https://github.com/Ravbug/sdl3-sample)


## Asset Credits
`PixelCode.ttf` - PixelCode Font v2.2 - [qwerasd205/PixelCode](https://github.com/qwerasd205/PixelCode)