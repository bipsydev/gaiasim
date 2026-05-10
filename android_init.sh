#!/bin/bash

# Create symlinks to libraries into the JNI directory so that the Android NDK can compile them
# Create symlinks for all SDL libraries
ln -srf -t android-project/app/jni/ external_libs/SDL*

# copy build.gradle into android-project/app/ to replace it
cp build.gradle android-project/app/build.gradle