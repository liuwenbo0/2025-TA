#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include "include/ExtMonotonicStack.hpp"

using namespace std;
using namespace std::chrono;

bool runSingleTest(int testCase) {
    string inputFile = "testcases/monostack/input" + to_string(testCase) + ".txt";
    string outputFile = "testcases/monostack/output" + to_string(testCase) + ".txt";
    
    ifstream input(inputFile);
    ifstream expectedOutput(outputFile);
    
    if (!input.is_open() || !expectedOutput.is_open()) {
        cout << "测试用例 " << testCase << ": 无法打开文件" << endl;
        return false;
    }
    
    int n;
    input >> n;
    
    vector<int> nums(n);
    for (int i = 0; i < n; i++) {
        input >> nums[i];
    }
    
    vector<int> expected(n);
    for (int i = 0; i < n; i++) {
        expectedOutput >> expected[i];
    }
    
    input.close();
    expectedOutput.close();
    
    // 执行算法
    auto startTime = high_resolution_clock::now();
    vector<int> result = ExtMonotonicStack::nextGreaterElements(nums);
    auto endTime = high_resolution_clock::now();
    
    auto duration = duration_cast<microseconds>(endTime - startTime);
    
    // 验证结果
    if (result.size() != expected.size()) {
        cout << "测试用例 " << testCase << ": 结果数组大小不匹配" << endl;
        return false;
    }
    
    for (int i = 0; i < n; i++) {
        if (result[i] != expected[i]) {
            cout << "测试用例 " << testCase << ": 第 " << i << " 个位置结果不匹配" << endl;
            cout << "输入数组: [";
            for (int j = 0; j < n; j++) {
                cout << nums[j];
                if (j < n-1) cout << ", ";
            }
            cout << "]" << endl;
            cout << "期望结果: " << expected[i] << ", 实际结果: " << result[i] << endl;
            return false;
        }
    }
    
    // 额外验证：与暴力解法比较
    vector<int> bruteForceResult = ExtMonotonicStack::nextGreaterElementsBruteForce(nums);
    for (int i = 0; i < n; i++) {
        if (result[i] != bruteForceResult[i]) {
            cout << "测试用例 " << testCase << ": 与暴力解法结果不一致" << endl;
            return false;
        }
    }
    
    cout << "测试用例 " << testCase << ": 通过 (数组大小: " << n 
         << ", 用时: " << duration.count() << " 微秒)" << endl;
    
    return true;
}

void performanceTest() {
    cout << "\n=== 性能测试 ===" << endl;
    
    vector<int> sizes = {1000, 5000, 10000, 50000, 100000};
    
    for (int size : sizes) {
        vector<int> nums(size);
        for (int i = 0; i < size; i++) {
            nums[i] = rand() % 1000;
        }
        
        // 测试单调栈算法
        auto startTime = high_resolution_clock::now();
        vector<int> result = ExtMonotonicStack::nextGreaterElements(nums);
        auto endTime = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(endTime - startTime);
        
        cout << "数组大小 " << size << ": " << duration.count() << " 微秒" << endl;
        
        // 验证时间复杂度确实是 O(n)
        if (size <= 10000) {  // 只对小数据进行暴力验证
            auto bruteForceDuration = high_resolution_clock::now();
            vector<int> bruteResult = ExtMonotonicStack::nextGreaterElementsBruteForce(nums);
            auto bruteForceEnd = high_resolution_clock::now();
            auto bruteDuration = duration_cast<microseconds>(bruteForceEnd - bruteForceDuration);
            
            cout << "  暴力解法用时: " << bruteDuration.count() << " 微秒" << endl;
            cout << "  加速比: " << (double)bruteDuration.count() / duration.count() << "x" << endl;
        }
    }
}

int main() {
    cout << "=== 单调栈判题脚本（新测试数据集）===" << endl;
    cout << "测试右侧第一个更大元素算法..." << endl << endl;
    
    int passedTests = 0;
    int totalTests = 10;
    
    for (int i = 1; i <= totalTests; i++) {
        if (runSingleTest(i)) {
            passedTests++;
        }
    }
    
    cout << endl << "=== 基本测试结果 ===" << endl;
    cout << "通过测试: " << passedTests << "/" << totalTests << endl;
    
    if (passedTests == totalTests) {
        cout << "🎉 所有基本测试通过！" << endl;
        performanceTest();
    } else {
        cout << "❌ 部分测试失败，请检查实现。" << endl;
    }
    
    return 0;
}