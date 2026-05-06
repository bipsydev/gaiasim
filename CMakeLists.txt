cmake_minimum_required(VERSION 3.16)
project(pvzascii LANGUAGES C CXX)

# Set C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sets the output for the executable runtime binary and for shared libraries (.so or .dll files)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Compile the SDL libraries as CMake submodules
# EXCULDE_FROM_ALL should help so it only compiles when necessary
add_subdirectory(external_libs/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external_libs/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(external_libs/SDL_mixer EXCLUDE_FROM_ALL)
add_subdirectory(external_libs/SDL_ttf EXCLUDE_FROM_ALL)


# Collect all C++ source files in src/ into PROJECT_SOURCES
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
# TODO test if this is necessary for both adding & removing asset files without source file modification
file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/assets/*")

# Create the executable build target using the found C++ sources.
# If no .cpp files were found, fall back to example.c
if(PROJECT_SOURCES)
    # WIN32 tells the windows build it's a GUI application without the need for a command prompt window
    add_executable(pvzascii WIN32 ${PROJECT_SOURCES})
    target_include_directories(pvzascii PRIVATE ./include)
else()
    message(WARNING "No .cpp sources found in ${CMAKE_SOURCE_DIR}/src; falling back to example.c")
    add_executable(pvzascii WIN32 example.c)
endif()

# Link to the actual SDL3 libraries
target_link_libraries(pvzascii PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_mixer::SDL3_mixer
    SDL3_ttf::SDL3_ttf
)


# Copy assets folder to the output directory
# ALL allows this to run every time, even if sources are untouched
add_custom_target(force_copy_config ALL
    COMMAND ${CMAKE_COMMAND} -E rm -rf "$<TARGET_FILE_DIR:pvzascii>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:pvzascii>/assets"
    COMMENT "Copying assets folder to executable target directory"
)


#[[
cmake_minimum_required(VERSION 3.20)
project(gaiasim)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../build_output/desktop)

# Add include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add raylib subdirectory
add_subdirectory(external_libs/raylib)

# Collect all source files
file(GLOB SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.c"
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link raylib
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Include raylib headers
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external_libs/raylib/src)
#]]