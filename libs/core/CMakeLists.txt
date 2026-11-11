add_library(labyrinthCore STATIC
    src/mazeGrid.cpp
    src/mazeGenerator.cpp
    src/mazeSolver.cpp
    src/gameSession.cpp)

target_include_directories(labyrinthCore
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(labyrinthCore PUBLIC cxx_std_23)


set_target_properties(labyrinthCore PROPERTIES POSITION_INDEPENDENT_CODE ON)

applyProjectWarnings(labyrinthCore)
