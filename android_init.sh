#!/bin/bash

# This script initializes an Android project from the provided SDL template project.

# Clean and copy over the android-project starter template from SDL
# Good idea to clean the folder for old build files that might interfere
rm -rf android-project
# Copy the template project directory
# DO NOT try symlinking this as Ninja (from CMake) will throw a fit about symlinks
cp -r external_libs/SDL/android-project/ android-project

# Create symlinks to libraries into the JNI directory so that the Android NDK can compile them
# Create symlinks for all SDL libraries
# This is OK with Ninja
ln -srf -t android-project/app/jni/ external_libs/SDL*

# copy my project's build.gradle into android-project/app/ to replace it
# This adds the asset directory and enables CMake building
# refer to this for more info: https://github.com/libsdl-org/SDL/blob/main/docs/README-android.md
cp build.gradle android-project/app/build.gradle