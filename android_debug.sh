#!/bin/bash

# This script will build, install, and run an Android APK using Gradle and adb.
# Make sure that `android_init.sh` has been run first to set up the android-project directory and configuration.
# Should automatically close when logcat detects that the app has terminated. (not still in background)

# Change to the Gradle project directory
cd android-project/ || exit 1

# Build the APK (uses the build.gradle config we copied over in `android_init.sh`)
echo "Building APK..."
./gradlew assembleDebug || exit 1

# Install the APK (must connect a device with adb first)
echo "Installing APK..."
adb install -r ./app/build/outputs/apk/debug/app-debug.apk || (echo "APK install failed. Did you connect a device with adb yet?" && sleep 5 && exit 1)

# Start the app
# The package and activity name are hardcoded here, change if needed later!
echo "Starting app..."
adb shell am start -n org.libsdl.app/.SDLActivity || exit 1

# Wait a sec for app to fully start
echo "Waiting for app to start..."
sleep 1

# Get PID and start logcat
echo "Starting logcat..."
PID=$(adb shell pidof org.libsdl.app | tr -d '\r' | xargs)

if [ -z "$PID" ]; then
    echo "Error: Could not find PID for org.libsdl.app"
    sleep 5
    exit 1
fi

echo "Found PID: $PID"
echo "Starting logcat (will stop automatically when app terminates)..."

# Start logcat in the background
adb logcat --pid=$PID &
LOGCAT_PID=$!

# Monitor the app process
while kill -0 $LOGCAT_PID 2>/dev/null; do
    # Check if the app is still running
    CURRENT_PID=$(adb shell pidof org.libsdl.app | tr -d '\r' | xargs)
    
    if [ -z "$CURRENT_PID" ]; then
        echo "App terminated, stopping logcat..."
        kill $LOGCAT_PID 2>/dev/null
        break
    fi
    
    sleep 1
done

echo "Logcat stopped."
