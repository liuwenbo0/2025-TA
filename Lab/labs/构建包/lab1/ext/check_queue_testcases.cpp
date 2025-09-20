#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <chrono>
#include "include/ExtQueueTwoStacks.hpp"

using namespace std;
using namespace std::chrono;

bool runSingleTest(int testCase) {
    string inputFile = "testcases/queue/input" + to_string(testCase) + ".txt";
    string outputFile = "testcases/queue/output" + to_string(testCase) + ".txt";
    
    ifstream input(inputFile);
    ifstream expectedOutput(outputFile);
    
    if (!input.is_open() || !expectedOutput.is_open()) {
        cout << "测试用例 " << testCase << ": 无法打开文件" << endl;
        return false;
    }
    
    ExtQueueTwoStacks myQueue;
    queue<int> stdQueue;  // 标准队列用于验证
    
    int numOps;
    input >> numOps;
    
    vector<string> actualOutput;
    vector<string> expectedOutputLines;
    
    // 读取期望输出
    string line;
    while (getline(expectedOutput, line)) {
        if (!line.empty()) {
            expectedOutputLines.push_back(line);
        }
    }
    expectedOutput.close();
    
    // 执行操作
    auto startTime = high_resolution_clock::now();
    
    for (int i = 0; i < numOps; i++) {
        string operation;
        input >> operation;
        
        try {
            if (operation == "push") {
                int value;
                input >> value;
                myQueue.push(value);
                stdQueue.push(value);
            }
            else if (operation == "pop") {
                if (!myQueue.empty()) {
                    myQueue.pop();
                    stdQueue.pop();
                }
            }
            else if (operation == "front") {
                if (!myQueue.empty()) {
                    int result = myQueue.front();
                    actualOutput.push_back(to_string(result));
                    
                    // 验证与标准队列的一致性
                    if (result != stdQueue.front()) {
                        cout << "测试用例 " << testCase << ": 结果不一致！" << endl;
                        return false;
                    }
                }
            }
        }
        catch (const exception& e) {
            cout << "测试用例 " << testCase << ": 运行时错误 - " << e.what() << endl;
            return false;
        }
    }
    
    auto endTime = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(endTime - startTime);
    
    // 比较输出
    if (actualOutput.size() != expectedOutputLines.size()) {
        cout << "测试用例 " << testCase << ": 输出行数不匹配 (期望:" << expectedOutputLines.size() 
             << ", 实际:" << actualOutput.size() << ")" << endl;
        return false;
    }
    
    for (int i = 0; i < actualOutput.size(); i++) {
        if (actualOutput[i] != expectedOutputLines[i]) {
            cout << "测试用例 " << testCase << ": 第 " << (i+1) << " 行输出不匹配" << endl;
            cout << "期望: " << expectedOutputLines[i] << endl;
            cout << "实际: " << actualOutput[i] << endl;
            return false;
        }
    }
    
    cout << "测试用例 " << testCase << ": 通过 (用时: " << duration.count() << " 微秒)" << endl;
    return true;
}

int main() {
    cout << "=== 队列实现判题脚本（新测试数据集）===" << endl;
    cout << "测试用两个栈实现的队列..." << endl << endl;
    
    int passedTests = 0;
    int totalTests = 10;
    
    for (int i = 1; i <= totalTests; i++) {
        if (runSingleTest(i)) {
            passedTests++;
        }
    }
    
    cout << endl << "=== 测试结果 ===" << endl;
    cout << "通过测试: " << passedTests << "/" << totalTests << endl;
    
    if (passedTests == totalTests) {
        cout << "🎉 所有测试通过！队列实现正确！" << endl;
    } else {
        cout << "❌ 部分测试失败，请检查实现。" << endl;
    }
    
    return 0;
}