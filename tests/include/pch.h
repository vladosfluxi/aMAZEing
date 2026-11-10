// The precompiled header of the test project, exactly like Visual Studio
// creates it for a native unit test project.
//
// It brings in the unit test framework and the headers that every test file
// needs, so the single test files stay short.
#pragma once

// Visual Studio has the real framework; every other compiler uses the small
// stand-in that lives beside this file.
#if defined(_MSC_VER) && __has_include(<CppUnitTest.h>)
#include <CppUnitTest.h>

// Visual Studio finds the test classes by itself, so nothing has to be
// registered by hand there.
#define REGISTER_TEST_CLASS(className)
#else
#include "cppUnitTestShim.h"
#endif

#include <string>
#include <vector>
