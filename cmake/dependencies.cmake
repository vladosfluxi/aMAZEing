# Both libraries are first looked for on the computer. When they are missing,
# CMake downloads them while configuring, so the project builds on Windows,
# Linux and macOS without preparing anything by hand.
include(FetchContent)

# nlohmann/json: the JSON files of the game.
find_package(nlohmann_json 3.11 QUIET)

if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann/json was not found on this computer, it will be downloaded")
    set(JSON_BuildTests OFF CACHE INTERNAL "")
    FetchContent_Declare(nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
        URL_HASH SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(nlohmann_json)
else()
    message(STATUS "nlohmann/json was found on this computer: ${nlohmann_json_VERSION}")
endif()

# raylib: window, drawing and keyboard.
find_package(raylib 5.0 QUIET)

if(NOT raylib_FOUND)
    message(STATUS "raylib was not found on this computer, it will be downloaded")
    set(BUILD_EXAMPLES OFF CACHE INTERNAL "")
    set(BUILD_GAMES OFF CACHE INTERNAL "")
    set(CMAKE_POLICY_VERSION_MINIMUM 3.10 CACHE INTERNAL "")

    # The released archive instead of the git repository, so git does not have
    # to be installed. The hash makes sure we get the file we expect.
    FetchContent_Declare(raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
        URL_HASH SHA256=aea98ecf5bc5c5e0b789a76de0083a21a70457050ea4cc2aec7566935f5e258e
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(raylib)
else()
    message(STATUS "raylib was found on this computer: ${raylib_VERSION}")
endif()
