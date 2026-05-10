#!/bin/bash

# Clean and copy over the android-project starter template from SDL
rm -rf android-project
cp -r external_libs/SDL/android-project/ android-project

# Create symlinks to libraries into the JNI directory so that the Android NDK can compile them
# Create symlinks for all SDL libraries
ln -srf -t android-project/app/jni/ external_libs/SDL*

# copy build.gradle into android-project/app/ to replace it
cp build.gradle android-project/app/build.gradle