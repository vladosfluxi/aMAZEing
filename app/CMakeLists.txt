# Layer 3: the program the player starts, and the only place where drawing and
# keyboard code is allowed.
add_executable(labyrinth
    src/main.cpp
    src/gameApp.cpp
    src/mazeRenderer.cpp
    src/uiScreens.cpp
    src/inputReader.cpp)

target_include_directories(labyrinth PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(labyrinth PRIVATE cxx_std_23)

target_link_libraries(labyrinth PRIVATE
    labyrinthCore
    labyrinthStorage
    raylib)

applyProjectWarnings(labyrinth)

# On Windows the DLL files have to sit next to the .exe.
if(WIN32)
    add_custom_command(TARGET labyrinth POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:labyrinth> $<TARGET_FILE_DIR:labyrinth>
        COMMAND_EXPAND_LISTS)
endif()

# cmake --build build --target run
add_custom_target(run
    COMMAND $<TARGET_FILE:labyrinth>
    DEPENDS labyrinth
    WORKING_DIRECTORY $<TARGET_FILE_DIR:labyrinth>
    COMMENT "Starting Labyrinth")
