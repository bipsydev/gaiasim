cmake_minimum_required(VERSION 3.16)
project(gaiasim LANGUAGES C CXX)

# Set C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set C standard to C23
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Force debug symbols and disable optimizations for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  if(MSVC)
    # For Visual Studio, use /Zi for debug symbols and disable optimizations
    add_compile_options(/Zi /Od)
  else()
    # For other compilers, use -g for debug symbols and disable optimizations
    add_compile_options(-g -O0)
  endif()
endif()

if ((APPLE AND NOT CMAKE_SYSTEM_NAME MATCHES "Darwin") OR EMSCRIPTEN)
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")    # Disable shared builds on platforms where it does not make sense to use them
    set(SDL_SHARED OFF)
else()
    set(SDL_SHARED ON)
endif()

if(MSVC)
    if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
        add_definitions(/MP)				# parallelize each target, unless Ninja is the generator
    endif()
endif()



# Sets the output for the executable runtime binary and for shared libraries (.so or .dll files)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../build_output/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../build_output/$<CONFIGURATION>/libs/")

# Compile the SDL libraries as CMake submodules
# EXCULDE_FROM_ALL should help so it only compiles when necessary
#[[
add_subdirectory(external_libs/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external_libs/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(external_libs/SDL_mixer EXCLUDE_FROM_ALL)
add_subdirectory(external_libs/SDL_ttf EXCLUDE_FROM_ALL)
]]#


# Collect all C++ source files in src/ into PROJECT_SOURCES
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
# TODO test if this is necessary for both adding & removing asset files without source file modification
file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/assets/*")


# Set the name of the executable
set(EXECUTABLE_NAME ${PROJECT_NAME})


# Create the executable build target using the found C++ sources.
# If no .cpp files were found, fall back to example.c
if(PROJECT_SOURCES)

  # Create an executable or a shared library based on the platform and add our sources to it
  if (ANDROID)
    # The SDL java code is hardcoded to load libmain.so on android, so we need to change EXECUTABLE_NAME
    set(EXECUTABLE_NAME main)
    add_library(${EXECUTABLE_NAME} SHARED ${PROJECT_SOURCES})
  else()

    # WIN32 tells the windows build it's a GUI application without the need for a command prompt window
    add_executable(${EXECUTABLE_NAME} WIN32 ${PROJECT_SOURCES})
  
  endif()

  # Add include directories for all platforms
  target_include_directories(${EXECUTABLE_NAME} PRIVATE ./include)
  
  # Treat external library headers as system includes (suppresses strict warnings for them)
  target_include_directories(${EXECUTABLE_NAME} SYSTEM PRIVATE
    "${CMAKE_SOURCE_DIR}/external_libs/entt/src"
  )

  # Apply -Weffc++ only to our source files, not external libraries
  if(NOT MSVC)
    # TODO: remove `-Wno-format-security` once the logging in `SDL_Utils.hpp` is refactored to use `std::format` instead.
    target_compile_options(${EXECUTABLE_NAME} PRIVATE -Wno-format-security)
    # Apply -Weffc++ only to our own source files
    set_source_files_properties(${PROJECT_SOURCES} PROPERTIES COMPILE_FLAGS "-Weffc++")
  endif()

else()
  message(WARNING "No .cpp sources found in ${CMAKE_SOURCE_DIR}/src; falling back to example.c")
  add_executable(${PROJECT_NAME} WIN32 example.c)
endif()


# Specify C++23 standard (ensures it's applied to the target)
target_compile_features(${EXECUTABLE_NAME} PUBLIC cxx_std_23)

# on Web targets, we need CMake to generate a HTML webpage. 
if(EMSCRIPTEN)
	set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()



# Compile the SDL libraries as CMake submodules
# EXCULDE_FROM_ALL should help so it only compiles when necessary


# Configure SDL by calling its CMake file.
# we use EXCLUDE_FROM_ALL so that its install targets and configs don't
# pollute upwards into our configuration.
add_subdirectory(external_libs/SDL EXCLUDE_FROM_ALL)

# If you don't want SDL_ttf, then remove this section.
set(SDLTTF_VENDORED ON) # tell SDL_ttf to build its own dependencies
add_subdirectory(external_libs/SDL_ttf EXCLUDE_FROM_ALL)	

# SDL_mixer (used for playing audio)
set(SDLMIXER_MIDI_NATIVE OFF)     # disable formats we don't use to make the build faster and smaller. Also some of these don't work on all platforms so you'll need to do some experimentation.
set(SDLMIXER_GME OFF)
set(SDLMIXER_WAVPACK OFF)     
set(SDLMIXER_MOD OFF)
set(SDLMIXER_OPUS OFF)
set(SDLMIXER_MP3_MPG123 OFF)
set(SDLMIXER_VORBIS_VORBISFILE OFF)
set(SDLMIXER_FLAC_LIBFLAC OFF)
set(SDLMIXER_VENDORED ON)   # tell SDL_mixer to build its own dependencies
add_subdirectory(external_libs/SDL_mixer EXCLUDE_FROM_ALL)

# SDL_image (used for loading various image formats)
set(SDLIMAGE_VENDORED ON)
set(SDLIMAGE_AVIF OFF)	# disable formats we don't use to make the build faster and smaller.
set(SDLIMAGE_BMP OFF)
set(SDLIMAGE_JPEG OFF)
set(SDLIMAGE_WEBP OFF)
add_subdirectory(external_libs/SDL_image EXCLUDE_FROM_ALL)


# Add EnTT with Natvis support (VS Code's data visualizer).
set(ENTT_INSTALL ON)
set(ENTT_INCLUDE_NATVIS ON)
set(ENTT_CMAKELISTS "${CMAKE_CURRENT_LIST_DIR}/external_libs/entt/CMakeLists.txt")
if(NOT EXISTS "${ENTT_CMAKELISTS}")
    message(FATAL_ERROR
        "Missing EnTT submodule at external_libs/entt. "
        "Run: git submodule update --init --recursive")
endif()
add_subdirectory(external_libs/entt EXCLUDE_FROM_ALL)


# Zlib's CMakeLists.txt renames zconf.h to zconf.h.included to prevent CMake from
# regenerating it. However, this modifies the vendored source tree and shows as modified
# in git. Restore the original filename after configuration to keep the submodule clean.
set(ZLIB_HEADER "${CMAKE_SOURCE_DIR}/external_libs/SDL_image/external/zlib/zconf.h")
set(ZLIB_HEADER_INCLUDED "${CMAKE_SOURCE_DIR}/external_libs/SDL_image/external/zlib/zconf.h.included")
if(EXISTS "${ZLIB_HEADER_INCLUDED}" AND NOT EXISTS "${ZLIB_HEADER}")
    message(STATUS "Restoring zconf.h in SDL_image/zlib to keep git submodule clean")
    file(RENAME "${ZLIB_HEADER_INCLUDED}" "${ZLIB_HEADER}")
endif()

# Link to the actual SDL3 libraries
target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    SDL3_image::SDL3_image
    SDL3_mixer::SDL3_mixer
    SDL3_ttf::SDL3_ttf
    SDL3::SDL3
    # Also link to EnTT
    EnTT::EnTT
)


# SDL_Image bug: https://github.com/libsdl-org/SDL_image/issues/506
if (APPLE AND NOT BUILD_SHARED_LIBS)
    find_library(IO_LIB ImageIO REQUIRED)
    find_library(CS_LIB CoreServices REQUIRED)
    find_library(CT_LIB CoreText REQUIRED)
    find_library(CG_LIB CoreGraphics REQUIRED)
    find_library(CF_LIB CoreFoundation REQUIRED)
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${CF_LIB} ${CT_LIB} ${IO_LIB} ${CS_LIB} ${CG_LIB})
endif()

# Dealing with assets
# We have some non-code resources that our application needs in order to work. How we deal with those differs per platform.
if (APPLE)
    # on Apple targets, the application bundle has a "resources" subfolder where we can place our assets.
    # SDL_GetBasePath() gives us easy access to that location.
    set(input_root "${CMAKE_CURRENT_LIST_DIR}/src")
    macro(add_resource FILE)
        file(RELATIVE_PATH relpath "${input_root}" "${FILE}")
        get_filename_component(relpath "${relpath}" DIRECTORY)
        target_sources(${EXECUTABLE_NAME} PRIVATE ${FILE})
        set_property(SOURCE ${FILE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${relpath}")
    endmacro()
    add_resource("${CMAKE_CURRENT_LIST_DIR}/src/Inter-VariableFont.ttf")
    add_resource("${CMAKE_CURRENT_LIST_DIR}/src/the_entertainer.ogg")
    add_resource("${CMAKE_CURRENT_LIST_DIR}/src/gs_tiger.svg")
elseif(EMSCRIPTEN)
    # on the web, we have to put the files inside of the webassembly
    # somewhat unintuitively, this is done via a linker argument.
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE 
        "--preload-file \"${CMAKE_CURRENT_LIST_DIR}/src/Inter-VariableFont.ttf@/\""
        "--preload-file \"${CMAKE_CURRENT_LIST_DIR}/src/the_entertainer.ogg@/\""
        "--preload-file \"${CMAKE_CURRENT_LIST_DIR}/src/gs_tiger.svg@/\""
    )
else()
    if (ANDROID)
        if (NOT MOBILE_ASSETS_DIR)
            message(FATAL_ERROR "Building for Android, but MOBILE_ASSETS_DIR is not set")
        endif()
        file(MAKE_DIRECTORY "${MOBILE_ASSETS_DIR}")    # we need to create the project Assets dir otherwise CMake won't copy our assets.
    endif()

    
    macro(copy_helper filename)
        if (ANDROID)
            # MOBILE_ASSETS_DIR is set in the gradle file via the cmake command line and points to the Android Studio Assets folder. 
            # when we copy assets there, the Android build pipeline knows to add them to the apk. 
            set(outname "${MOBILE_ASSETS_DIR}/${filename}")    
        else()
            # for platforms that do not have a good packaging format, all we can do is copy the assets to the process working directory.
            set(outname "${CMAKE_BINARY_DIR}/$<CONFIGURATION>/${filename}")
        endif()
        add_custom_command(POST_BUILD
            TARGET "${EXECUTABLE_NAME}"
		    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/src/${filename}" "${outname}"
	    )
    endmacro()
endif()

# set some extra configs for each platform
set(RESOURCE_FILES "src/logo.png")
if(IOS)
    set(RESOURCE_FILES "src/iosLaunchScreen.storyboard;src/logo.png") # make the splash screen show up on iOS
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES 
    # On macOS, make a proper .app bundle instead of a bare executable
    MACOSX_BUNDLE TRUE
    # Set the Info.plist file for Apple Mobile platforms. Without this file, your app
    # will not launch. 
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/src/Info.plist.in"

    # in Xcode, create a Scheme in the schemes dropdown for the app.
    XCODE_GENERATE_SCHEME TRUE
    # Identification for Xcode
    XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.ravbug.sdl3-sample"
	XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.ravbug.sdl3-sample"
	XCODE_ATTRIBUTE_CURRENTYEAR "${CURRENTYEAR}"
    RESOURCE "${RESOURCE_FILES}"
)

# on Visual Studio, set our app as the default project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${EXECUTABLE_NAME}")

# On macOS Platforms, ensure that the bundle is valid for distribution by calling fixup_bundle.
# note that fixup_bundle does not work on iOS, so you will want to use static libraries 
# or manually copy dylibs and set rpaths
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    # tell Install about the target, otherwise fixup won't know about the transitive dependencies
    install(TARGETS ${EXECUTABLE_NAME}
    	BUNDLE DESTINATION ./install COMPONENT Runtime
   	    RUNTIME DESTINATION ./install/bin COMPONENT Runtime
    )
	
    set(DEP_DIR "${CMAKE_BINARY_DIR}")  # where to look for dependencies when fixing up
    INSTALL(CODE 
        "include(BundleUtilities)
        fixup_bundle(\"$<TARGET_BUNDLE_DIR:${EXECUTABLE_NAME}>\" \"\" \"${DEP_DIR}\")
        " 
    )
    set(CPACK_GENERATOR "DragNDrop")
    include(CPack)
endif()



# Copy assets folder to the output directory
# ALL allows this to run every time, even if sources are untouched
add_custom_target(force_copy_config ALL
  COMMAND ${CMAKE_COMMAND} -E rm -rf "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/assets"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_SOURCE_DIR}/assets"
  "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/assets"
  COMMENT "Copying assets folder to executable target directory"
)
