#include <iostream>
#include <vector>
#include <cassert>
#include "include/ExtQueueTwoStacks.hpp"
#include "include/ExtMonotonicStack.hpp"

using namespace std;

void testQueueTwoStacks() {
    cout << "=== 测试拓展题1：用两个栈实现队列 ===" << endl;
    
    ExtQueueTwoStacks queue;
    
    // 测试基本操作
    cout << "测试基本操作：" << endl;
    queue.push(1);
    queue.push(2);
    queue.push(3);
    
    cout << "队列大小: " << queue.size() << endl;  // 应该输出 3
    cout << "队列头部: " << queue.front() << endl;  // 应该输出 1
    
    queue.pop();
    cout << "pop 后队列头部: " << queue.front() << endl;  // 应该输出 2
    
    queue.push(4);
    cout << "push(4) 后队列头部: " << queue.front() << endl;  // 应该输出 2
    
    // 测试摊还复杂度
    cout << "\n测试摊还复杂度（大量操作）：" << endl;
    queue.clear();
    
    // 执行大量混合操作
    for (int i = 0; i < 1000; i++) {
        queue.push(i);
    }
    
    for (int i = 0; i < 500; i++) {
        queue.pop();
    }
    
    for (int i = 1000; i < 1500; i++) {
        queue.push(i);
    }
    
    cout << "大量操作后队列大小: " << queue.size() << endl;
    cout << "队列头部: " << queue.front() << endl;
    
    cout << "拓展题1测试完成！" << endl << endl;
}

void testMonotonicStack() {
    cout << "=== 测试拓展题2：单调栈：右侧第一个更大元素 ===" << endl;
    
    // 测试用例1
    vector<int> nums1 = {2, 1, 2, 4, 3, 1};
    vector<int> result1 = ExtMonotonicStack::nextGreaterElements(nums1);
    cout << "测试用例1：" << endl;
    ExtMonotonicStack::printResult(nums1, result1);
    // 期望结果: [3, 2, 3, -1, -1, -1]
    
    // 测试用例2
    vector<int> nums2 = {1, 3, 2, 4};
    vector<int> result2 = ExtMonotonicStack::nextGreaterElements(nums2);
    cout << "\n测试用例2：" << endl;
    ExtMonotonicStack::printResult(nums2, result2);
    // 期望结果: [1, 3, 3, -1]
    
    // 测试用例3：单调递增
    vector<int> nums3 = {1, 2, 3, 4, 5};
    vector<int> result3 = ExtMonotonicStack::nextGreaterElements(nums3);
    cout << "\n测试用例3（单调递增）：" << endl;
    ExtMonotonicStack::printResult(nums3, result3);
    // 期望结果: [1, 2, 3, 4, -1]
    
    // 测试用例4：单调递减
    vector<int> nums4 = {5, 4, 3, 2, 1};
    vector<int> result4 = ExtMonotonicStack::nextGreaterElements(nums4);
    cout << "\n测试用例4（单调递减）：" << endl;
    ExtMonotonicStack::printResult(nums4, result4);
    // 期望结果: [-1, -1, -1, -1, -1]
    
    // 验证正确性
    cout << "\n验证算法正确性：" << endl;
    vector<int> bruteForceResult1 = ExtMonotonicStack::nextGreaterElementsBruteForce(nums1);
    bool correct = (result1 == bruteForceResult1);
    cout << "算法结果与暴力解法一致: " << (correct ? "是" : "否") << endl;
    
    cout << "拓展题2测试完成！" << endl << endl;
}

int main() {
    cout << "=== Lab1 拓展题演示程序 ===" << endl << endl;
    
    testQueueTwoStacks();
    testMonotonicStack();
    
    cout << "所有测试完成！" << endl;
    return 0;
}