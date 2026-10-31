# Labyrinth - a maze game written in C++23 with raylib and nlohmann/json.
#
# The build has three steps and they always happen in this order:
#   1. labyrinthCore    is built as a STATIC library (the rules of the game)
#   2. labyrinthStorage is built as a SHARED library (the JSON files)
#   3. labyrinth        is the executable that links both libraries together
cmake_minimum_required(VERSION 3.21)

project(labyrinth
    VERSION 1.0.0
    DESCRIPTION "Maze game with a raylib interface and JSON save files"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# A build without a chosen type would have no optimisation at all.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type of the project" FORCE)
endif()

# The executable and the shared library end up in the same folder, so the
# program finds the library next to itself on every system.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Windows: the program and the shared library have to use the same C runtime.
# The library gives finished mazes to the program, so memory that the library
# allocated is freed by the program, and that only works with one shared
# runtime for both of them.
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# On Linux and macOS the executable looks for the shared library beside itself.
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
if(APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()

option(labyrinthBuildTests "Build the tests of the core library" ON)

include(cmake/projectWarnings.cmake)
include(cmake/dependencies.cmake)

add_subdirectory(libs/core)     # step 1: static library
add_subdirectory(libs/storage)  # step 2: shared library
add_subdirectory(app)           # step 3: executable

if(labyrinthBuildTests)
    enable_testing()
    add_subdirectory(tests)
endif()

message(STATUS "")
message(STATUS "Labyrinth ${PROJECT_VERSION} is ready to be built")
message(STATUS "  build type : ${CMAKE_BUILD_TYPE}")
message(STATUS "  programs   : ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "")
