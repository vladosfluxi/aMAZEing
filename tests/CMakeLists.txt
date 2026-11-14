# The tests of the project.

set(useVisualStudioTests OFF)

if(MSVC)
    get_filename_component(msvcCompilerDir "${CMAKE_CXX_COMPILER}" DIRECTORY)
    get_filename_component(visualStudioVcDir "${msvcCompilerDir}/../../../../../.." ABSOLUTE)

    find_path(cppUnitTestIncludeDir CppUnitTest.h
        HINTS "${visualStudioVcDir}/Auxiliary/VS/UnitTest/include"
              "$ENV{VCInstallDir}/Auxiliary/VS/UnitTest/include")

    find_library(cppUnitTestLibrary Microsoft.VisualStudio.TestTools.CppUnitTestFramework
        HINTS "${visualStudioVcDir}/Auxiliary/VS/UnitTest/lib/x64"
              "${visualStudioVcDir}/Auxiliary/VS/UnitTest/lib"
              "$ENV{VCInstallDir}/Auxiliary/VS/UnitTest/lib/x64")

    if(cppUnitTestIncludeDir AND cppUnitTestLibrary)
        set(useVisualStudioTests ON)
        message(STATUS "The unit test framework of Visual Studio was found, the tests become DLLs")
    else()
        message(STATUS "The unit test framework of Visual Studio was not found, the tests become programs")
    endif()
endif()

# ---------------------------------------------------------------------------
# One test target, built in the way that fits the compiler.
# ---------------------------------------------------------------------------
function(addLabyrinthTest targetName sourceFile libraryToTest)
    if(useVisualStudioTests)
        add_library(${targetName} SHARED ${sourceFile} src/pch.cpp)
        target_include_directories(${targetName} PRIVATE ${cppUnitTestIncludeDir})
        target_link_libraries(${targetName} PRIVATE ${libraryToTest} ${cppUnitTestLibrary})
    else()
        add_executable(${targetName} ${sourceFile} src/pch.cpp src/cppUnitTestShim.cpp src/testRunner.cpp)
        target_link_libraries(${targetName} PRIVATE ${libraryToTest})
        add_test(NAME ${targetName} COMMAND ${targetName})
    endif()

    target_include_directories(${targetName} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_compile_features(${targetName} PRIVATE cxx_std_23)

    target_precompile_headers(${targetName} PRIVATE include/pch.h)

    applyProjectWarnings(${targetName})
endfunction()

# The rules of the game can be tested without a window and without files.
addLabyrinthTest(coreTests src/coreTests.cpp labyrinthCore)

# The storage library is tested in a folder of its own, so the files of the
# player are never touched.
addLabyrinthTest(storageTests src/storageTests.cpp labyrinthStorage)
