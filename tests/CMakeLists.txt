# The tests of the project.
#
# The test files are written in the style of a native unit test project of
# Visual Studio: TEST_CLASS, TEST_METHOD and Assert.
#
# Visual Studio builds such a project as a DLL and its Test Explorer loads it.
# When its unit test framework is found, that is exactly what happens here as
# well. On every other compiler the same test files are built as a normal
# program that writes its results, so "ctest" works on Linux and macOS too.

# ---------------------------------------------------------------------------
# Is the unit test framework of Visual Studio available?
# ---------------------------------------------------------------------------
set(useVisualStudioTests OFF)

if(MSVC)
    # The framework lives next to the compiler, in the Visual Studio folder:
    #   <Visual Studio>/VC/Auxiliary/VS/UnitTest
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
        # Visual Studio loads a test project as a DLL and starts the test
        # methods by itself, so no main function is built in.
        add_library(${targetName} SHARED ${sourceFile} src/pch.cpp)
        target_include_directories(${targetName} PRIVATE ${cppUnitTestIncludeDir})
        target_link_libraries(${targetName} PRIVATE ${libraryToTest} ${cppUnitTestLibrary})
    else()
        # Everywhere else the tests are a normal program with its own runner.
        add_executable(${targetName} ${sourceFile} src/pch.cpp src/cppUnitTestShim.cpp src/testRunner.cpp)
        target_link_libraries(${targetName} PRIVATE ${libraryToTest})
        add_test(NAME ${targetName} COMMAND ${targetName})
    endif()

    target_include_directories(${targetName} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_compile_features(${targetName} PRIVATE cxx_std_23)

    # The precompiled header, just like a test project of Visual Studio has one.
    target_precompile_headers(${targetName} PRIVATE include/pch.h)

    applyProjectWarnings(${targetName})
endfunction()

# The rules of the game can be tested without a window and without files.
addLabyrinthTest(coreTests src/coreTests.cpp labyrinthCore)

# The storage library is tested in a folder of its own, so the files of the
# player are never touched.
addLabyrinthTest(storageTests src/storageTests.cpp labyrinthStorage)
