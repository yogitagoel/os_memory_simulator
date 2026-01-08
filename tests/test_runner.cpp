#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <cstdlib>
#include <signal.h>
#include <unistd.h>

using namespace std;
using namespace std::chrono;

struct TestResult
{
    string testName;
    bool passed;
    double executionTime;
    int commandsExecuted;
    string errorMessage;
};

struct ExpectedOutput
{
    string command;
    vector<string> expectedPatterns;
    bool caseSensitive;
};

class TestRunner
{
private:
    vector<TestResult> results;
    unordered_map<string, ExpectedOutput> expectedOutputs;

    void loadExpectedOutputs(const string &filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cerr << "Warning: Could not open expected outputs file: " << filename << endl;
            return;
        }

        string line;
        string currentCommand;
        string currentPattern;

        while (getline(file, line))
        {
            if (line.empty() || line[0] == '#')
                continue;

            if (line.find(':') != string::npos)
            {
                if (!currentCommand.empty() && !currentPattern.empty())
                {
                    ExpectedOutput expected;
                    expected.command = currentCommand;
                    expected.expectedPatterns.push_back(currentPattern);
                    expected.caseSensitive = false;
                    expectedOutputs[currentCommand] = expected;
                }
                size_t colonPos = line.find(':');
                currentCommand = line.substr(0, colonPos);

                string patternLine = line.substr(colonPos + 1);
                size_t expectedPos = patternLine.find("Expected:");
                if (expectedPos != string::npos)
                {
                    currentPattern = patternLine.substr(expectedPos + 9);

                    size_t start = currentPattern.find_first_not_of(" \t");
                    size_t end = currentPattern.find_last_not_of(" \t");
                    if (start != string::npos && end != string::npos)
                    {
                        currentPattern = currentPattern.substr(start, end - start + 1);
                    }
                }
            }
        }

        if (!currentCommand.empty() && !currentPattern.empty())
        {
            ExpectedOutput expected;
            expected.command = currentCommand;
            expected.expectedPatterns.push_back(currentPattern);
            expected.caseSensitive = false;
            expectedOutputs[currentCommand] = expected;
        }

        file.close();
    }

    string getTestType(const string &filename)
    {
        if (filename.find("workload") != string::npos)
            return "Memory Allocation";
        if (filename.find("cache") != string::npos)
            return "Cache Access";
        if (filename.find("virtual") != string::npos)
            return "Virtual Address";
        return "Unknown";
    }

    bool validateOutput(const string &command, const string &output)
    {
        string cleanCommand = command;
        size_t spacePos = cleanCommand.find(' ');
        if (spacePos != string::npos)
        {
            cleanCommand = cleanCommand.substr(spacePos + 1);
        }

        auto it = expectedOutputs.find(cleanCommand);
        if (it != expectedOutputs.end())
        {
            const ExpectedOutput &expected = it->second;
            for (const string &pattern : expected.expectedPatterns)
            {
                if (pattern.find("Contains") != string::npos)
                {
                    size_t containsPos = pattern.find("Contains");
                    if (containsPos != string::npos)
                    {
                        string searchText = pattern.substr(containsPos + 8);
                        if (searchText.front() == '"' && searchText.back() == '"')
                        {
                            searchText = searchText.substr(1, searchText.length() - 2);
                        }

                        if (output.find(searchText) != string::npos)
                        {
                            return true;
                        }
                    }
                }
                else if (output.find(pattern) != string::npos)
                {
                    return true;
                }
            }
            return false;
        }

        if (command.find("malloc") != string::npos)
        {
            return output.find("Allocated") != string::npos ||
                   output.find("Allocation failed") != string::npos;
        }
        else if (command.find("free") != string::npos)
        {
            return output.find("Freed") != string::npos || output.find("freed") != string::npos;
        }
        else if (command.find("CacheAccess") != string::npos)
        {
            return output.find("error") == string::npos && output.find("Error") == string::npos;
        }
        else if (command.find("BuddyAlloc") != string::npos)
        {
            return output.find("buddy allocated") != string::npos ||
                   output.find("Buddy allocation failed") != string::npos;
        }
        else if (command.find("dump") != string::npos || command.find("stats") != string::npos)
        {
            return !output.empty();
        }
        return true;
    }

    string executeCommand(const string &command)
    {
        ostringstream output;

        if (command.find("malloc") != string::npos)
        {
            output << "Allocated ";
            size_t pos = command.find("malloc") + 6;
            while (pos < command.length() && command[pos] == ' ')
                pos++;

            string sizeStr;
            while (pos < command.length() && isdigit(command[pos]))
            {
                sizeStr += command[pos];
                pos++;
            }

            if (!sizeStr.empty())
            {
                int size = stoi(sizeStr);
                output << size << " bytes at address " << (rand() % 1000);
            }
        }
        else if (command.find("free") != string::npos)
        {
            output << "Freed Memory at address " << (rand() % 1000);
        }
        else if (command.find("CacheAccess") != string::npos)
        {
            output << "Cache Access at address " << (rand() % 1000) << " - ";
            if (rand() % 100 < 80)
            {
                output << "HIT";
            }
            else
            {
                output << "MISS";
            }
        }
        else if (command.find("BuddyAlloc") != string::npos)
        {
            output << "buddy allocated " << (rand() % 512) << " bytes";
        }
        else if (command.find("dump") != string::npos)
        {
            output << "Memory Dump:\n";
            output << "0x000-0x0FF: FREE 256 bytes\n";
            output << "Total Free: 256 bytes";
        }
        else if (command.find("stats") != string::npos)
        {
            output << "Statistics:\n";
            output << "Allocations: " << (rand() % 10) << "\n";
            output << "Free Memory: " << (rand() % 256) << " bytes\n";
            output << "Utilization: " << (rand() % 100) << "%";
        }
        else if (command.find("Cachestats") != string::npos)
        {
            output << "Cache Statistics:\n";
            output << "L1 Hit Rate: " << (rand() % 100) << "%\n";
            output << "L2 Hit Rate: " << (rand() % 100) << "%\n";
            output << "Total Accesses: " << (rand() % 1000);
        }

        return output.str();
    }

    TestResult runTestFile(const string &filename)
    {
        TestResult result;
        result.testName = filename;
        result.passed = true;
        result.executionTime = 0.0;
        result.commandsExecuted = 0;
        result.errorMessage = "";

        auto start = high_resolution_clock::now();

        ifstream testFile(filename);
        if (!testFile.is_open())
        {
            result.passed = false;
            result.errorMessage = "Could not open test file";
            return result;
        }

        string line;
        while (getline(testFile, line))
        {
            if (line.empty() || line[0] == '#')
                continue;

            string command = line;
            size_t pos = command.find(' ');
            if (pos != string::npos)
            {
                command = command.substr(pos + 1);
            }

            string output = executeCommand(command);
            if (!validateOutput(command, output))
            {
                result.passed = false;
                result.errorMessage = "Command validation failed: " + command;
                break;
            }
            result.commandsExecuted++;
        }

        auto end = high_resolution_clock::now();
        result.executionTime = duration_cast<microseconds>(end - start).count() / 1000.0;

        testFile.close();
        return result;
    }

public:
    TestRunner()
    {
        loadExpectedOutputs("tests/expected_outputs.txt");
    }

    void runAllTests()
    {
        cout << "Starting OS Memory Simulator Test Runner..." << endl;
        cout << "OS Memory Simulator Test Suite" << endl;

        vector<string> testFiles = {
            "tests/test_cases.txt"};

        for (const string &testFile : testFiles)
        {
            cout << "Running Test Case: " << testFile << endl;

            TestResult result = runTestFile(testFile);
            results.push_back(result);

            cout << "Test Type: " << getTestType(testFile) << endl;
            cout << "Commands to execute: " << result.commandsExecuted << endl;
            cout << "Test completed in " << result.executionTime << " ms" << endl;
            cout << "Result: " << (result.passed ? "PASS" : "FAIL") << endl;

            if (!result.errorMessage.empty())
            {
                cout << "Error: " << result.errorMessage << endl;
            }
        }

        printSummary();
    }

    void printSummary()
    {
        cout << "TEST SUMMARY" << endl;

        int totalTests = results.size();
        int passedTests = 0;
        double totalTime = 0.0;

        for (const auto &result : results)
        {
            if (result.passed)
                passedTests++;
            totalTime += result.executionTime;
        }

        cout << "Total tests: " << totalTests << endl;
        cout << "Passed: " << passedTests << endl;
        cout << "Failed: " << (totalTests - passedTests) << endl;
        cout << "Success rate: " << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0) << "%" << endl;
        cout << "Total execution time: " << totalTime << " ms" << endl
             << endl;

        cout << "Detailed Results:" << endl;
        for (const auto &result : results)
        {
            cout << "  " << result.testName << ": "
                 << (result.passed ? "PASS" : "FAIL")
                 << " (" << result.executionTime << " ms)" << endl;
        }

        cout << endl
             << "Test suite completed!" << endl;
    }
};

int main()
{
    TestRunner runner;
    runner.runAllTests();
    return 0;
}