cmake_minimum_required(VERSION 3.20)
project(gaiasim)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

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
