// The precompiled header of the test project
#pragma once

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