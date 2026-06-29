set(CMAKE_BUILD_TYPE "Debug")

cmake_minimum_required(VERSION 3.21)
project(simple-triangle-opengl-sdl3-wasm-cpp)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the exact location of the library configuration files
# The variable name must strictly match: LibraryName_DIR
set(SDL3_DIR "C:/libs/SDL3-devel-3.4.10-wasm/lib/cmake/SDL3")

# Check for the presence of the library in the system
# If it is not found, CMake will abort the configuration with an error
# REQUIRED means the library is mandatory for the build
find_package(SDL3 REQUIRED)

# Set the name of the future application (on the web it will be app.js / app.wasm,
# while on Windows it would be app.exe)
add_executable(app)

# Add source code to the project
target_sources(app
    PRIVATE
    src/main.cpp
)

# Emscripten Linker Options for WebGL 2.0 / OpenGL ES 3.0
target_link_options(app PRIVATE
    # Use our own SDL3 build instead of Emscripten's internal version
    "-sUSE_SDL=0"

    # Force WebGL 2.0 (which corresponds to OpenGL ES 3.0)
    "-sMAX_WEBGL_VERSION=2"
    "-sMIN_WEBGL_VERSION=2"
    "-sFULL_ES3=1"
)

# Link the library to our application (configures linking and include paths)
target_link_libraries(app PRIVATE SDL3::SDL3)
