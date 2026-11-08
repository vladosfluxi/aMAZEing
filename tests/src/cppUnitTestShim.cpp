#include "cppUnitTestShim.h"

#include <exception>
#include <iostream>
#include <vector>

namespace Microsoft::VisualStudio::CppUnitTestFramework
{
    // The list of all test methods. It lives inside a function, so it surely
    // exists before the first test registers itself.
    static std::vector<TestMethodEntry>& allTestMethods()
    {
        static std::vector<TestMethodEntry> testMethods;
        return testMethods;
    }

    // The class whose methods are being registered right now.
    static std::string& currentTestClassName()
    {
        static std::string className;
        return className;
    }

    TestClassNameSetter::TestClassNameSetter(const char* className)
    {
        currentTestClassName() = className;
    }

    TestMethodRegistrar::TestMethodRegistrar(const char* methodName, void (*testFunction)())
    {
        allTestMethods().push_back(TestMethodEntry{currentTestClassName(), methodName, testFunction});
    }

    std::string messageText(const wchar_t* message)
    {
        std::string text;
        for (const wchar_t* character = message; character != nullptr && *character != L'\0'; ++character)
        {
            text.push_back(static_cast<char>(*character));
        }
        return text;
    }

    void failCheck(const char* checkName, const std::string& expectedText, const std::string& actualText,
                   const wchar_t* message)
    {
        std::string reason = std::string("Assert::") + checkName + " failed.";

        if (!expectedText.empty() || !actualText.empty())
        {
            reason += " Expected:<" + expectedText + "> Actual:<" + actualText + ">.";
        }

        const std::string ownMessage = messageText(message);
        if (!ownMessage.empty())
        {
            reason += " " + ownMessage;
        }

        throw TestFailure{reason};
    }

    int runAllTests()
    {
        int testsRun = 0;
        int testsFailed = 0;
        std::string writtenClassName;

        for (const TestMethodEntry& test : allTestMethods())
        {
            // The name of a class is written once, above its methods.
            if (test.testClassName != writtenClassName)
            {
                writtenClassName = test.testClassName;
                std::cout << writtenClassName << '\n';
            }

            ++testsRun;
            try
            {
                test.testFunction();
                std::cout << "  ok      " << test.testMethodName << '\n';
            }
            catch (const TestFailure& failure)
            {
                ++testsFailed;
                std::cout << "  FAILED  " << test.testMethodName << '\n'
                          << "          " << failure.reason << '\n';
            }
            catch (const std::exception& error)
            {
                ++testsFailed;
                std::cout << "  FAILED  " << test.testMethodName << '\n'
                          << "          unexpected exception: " << error.what() << '\n';
            }
        }

        std::cout << "\ntests run: " << testsRun << ", failed: " << testsFailed << '\n';
        return testsFailed == 0 ? 0 : 1;
    }
}
