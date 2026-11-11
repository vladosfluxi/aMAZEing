add_library(labyrinthStorage SHARED
    src/jsonFile.cpp
    src/dataFiles.cpp
    src/gameSettings.cpp
    src/scoreBoard.cpp
    src/savedGame.cpp)

target_include_directories(labyrinthStorage
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_features(labyrinthStorage PUBLIC cxx_std_23)


target_link_libraries(labyrinthStorage
    PUBLIC labyrinthCore
    PRIVATE nlohmann_json::nlohmann_json)


target_compile_definitions(labyrinthStorage PRIVATE labyrinthStorageIsBuilt)

set_target_properties(labyrinthStorage PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON)

applyProjectWarnings(labyrinthStorage)
