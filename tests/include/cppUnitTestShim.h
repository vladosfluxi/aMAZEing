// A small stand-in for "CppUnitTest.h", the unit test framework that comes
// with Visual Studio.
#pragma once

#include <sstream>
#include <string>

// This is the namespace of the Visual Studio framework. The tests say
// "using namespace Microsoft::VisualStudio::CppUnitTestFramework;" exactly as
// a test project of Visual Studio does, so the same line works in both cases.
namespace Microsoft::VisualStudio::CppUnitTestFramework
{
    struct TestFailure
    {
        std::string reason;
    };

    // Turns the wide message of a check into normal text.
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

    // The checks of the framework.
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

    // One of these is created for every TEST_METHOD and it puts the method
    // into the list of all tests.
    struct TestMethodRegistrar
    {
        TestMethodRegistrar(const char* methodName, void (*testFunction)());
    };

    // Runs every registered test method and writes the results.
    int runAllTests();
}

// TEST_CLASS opens a class, TEST_METHOD writes a test into it, and
// REGISTER_TEST_CLASS makes the tests of the class known.
#define TEST_CLASS(className) struct className

#define TEST_METHOD(methodName)                                                                    \
    ::Microsoft::VisualStudio::CppUnitTestFramework::TestMethodRegistrar methodName##Registrar{     \
        #methodName, &methodName};                                                                 \
    static void methodName()

#define REGISTER_TEST_CLASS(className)                                                             \
    static const ::Microsoft::VisualStudio::CppUnitTestFramework::TestClassNameSetter              \
        className##NameSetter(#className);                                                         \
    static className className##Instance
