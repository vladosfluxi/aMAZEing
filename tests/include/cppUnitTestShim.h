// A small stand-in for "CppUnitTest.h", the unit test framework that comes
// with Visual Studio.
//
// The test files of this project are written in the Visual Studio way, with
// TEST_CLASS, TEST_METHOD and Assert. On Windows with Visual Studio the real
// framework is used and the tests appear in the Test Explorer. Every other
// compiler uses this header instead, so the same test files also run on Linux
// and macOS, where ctest starts them.
//
// Only the part of the framework that this project needs is written here.
#pragma once

#include <sstream>
#include <string>

// This is the namespace of the Visual Studio framework. The tests say
// "using namespace Microsoft::VisualStudio::CppUnitTestFramework;" exactly as
// a test project of Visual Studio does, so the same line works in both cases.
namespace Microsoft::VisualStudio::CppUnitTestFramework
{
    // Thrown by a check that failed. The runner catches it, writes the reason
    // and goes on with the next test method.
    struct TestFailure
    {
        std::string reason;
    };

    // Turns the wide message of a check into normal text. The tests only use
    // plain letters, so taking the characters over one by one is enough.
    std::string messageText(const wchar_t* message);

    // Stops the running test method and reports why.
    void failCheck(const char* checkName, const std::string& expectedText, const std::string& actualText,
                   const wchar_t* message);

    // Writes a value as text, for the report of a failed check.
    template <typename ValueType>
    std::string valueText(const ValueType& value)
    {
        std::ostringstream textStream;
        textStream << value;
        return textStream.str();
    }

    // The checks of the framework. Every one of them ends the test method as
    // soon as it fails, exactly like in Visual Studio.
    class Assert
    {
    public:
        template <typename ValueType>
        static void AreEqual(const ValueType& expected, const ValueType& actual,
                             const wchar_t* message = nullptr)
        {
            if (!(expected == actual))
            {
                failCheck("AreEqual", valueText(expected), valueText(actual), message);
            }
        }

        static void AreEqual(const char* expected, const char* actual, const wchar_t* message = nullptr)
        {
            AreEqual(std::string(expected), std::string(actual), message);
        }

        // Two numbers with a comma are never exactly equal, so they are
        // compared with a tolerance.
        static void AreEqual(double expected, double actual, double tolerance,
                             const wchar_t* message = nullptr)
        {
            const double difference = expected > actual ? expected - actual : actual - expected;
            if (difference > tolerance)
            {
                failCheck("AreEqual", valueText(expected), valueText(actual), message);
            }
        }

        template <typename ValueType>
        static void AreNotEqual(const ValueType& notExpected, const ValueType& actual,
                                const wchar_t* message = nullptr)
        {
            if (notExpected == actual)
            {
                failCheck("AreNotEqual", "not " + valueText(notExpected), valueText(actual), message);
            }
        }

        static void IsTrue(bool condition, const wchar_t* message = nullptr)
        {
            if (!condition)
            {
                failCheck("IsTrue", "true", "false", message);
            }
        }

        static void IsFalse(bool condition, const wchar_t* message = nullptr)
        {
            if (condition)
            {
                failCheck("IsFalse", "false", "true", message);
            }
        }

        static void Fail(const wchar_t* message = nullptr)
        {
            failCheck("Fail", "", "", message);
        }
    };

    // One test method that was found in a test class.
    struct TestMethodEntry
    {
        std::string testClassName;
        std::string testMethodName;
        void (*testFunction)();
    };

    // Remembers which test class is being registered at the moment, so that
    // the methods of that class know the name they belong to.
    struct TestClassNameSetter
    {
        explicit TestClassNameSetter(const char* className);
    };

    // One of these is created for every TEST_METHOD, and it puts the method
    // into the list of all tests.
    struct TestMethodRegistrar
    {
        TestMethodRegistrar(const char* methodName, void (*testFunction)());
    };

    // Runs every registered test method and writes the results.
    // Gives back 0 when everything passed and 1 when something failed.
    int runAllTests();
}

// The three macros of the framework.
//
// TEST_CLASS opens a class, TEST_METHOD writes a test into it, and
// REGISTER_TEST_CLASS makes the tests of the class known. Visual Studio finds
// the classes by itself, so there the last macro is empty.
#define TEST_CLASS(className) struct className

#define TEST_METHOD(methodName)                                                                    \
    ::Microsoft::VisualStudio::CppUnitTestFramework::TestMethodRegistrar methodName##Registrar{     \
        #methodName, &methodName};                                                                 \
    static void methodName()

#define REGISTER_TEST_CLASS(className)                                                             \
    static const ::Microsoft::VisualStudio::CppUnitTestFramework::TestClassNameSetter              \
        className##NameSetter(#className);                                                         \
    static className className##Instance
