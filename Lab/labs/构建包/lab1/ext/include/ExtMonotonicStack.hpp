#ifndef EXTMONOTONICSTACK_HPP
#define EXTMONOTONICSTACK_HPP

#include <vector>
#include <stack>

/**
 * 拓展题2：单调栈：右侧第一个更大元素
 * 
 * 要求：给定数组，输出每个位置右侧第一个严格更大元素的下标（不存在则输出 -1）。
 * 时间复杂度 O(n)。
 * 
 * 算法思路：
 * 1. 使用单调递减栈（栈中存储数组下标）
 * 2. 从右到左遍历数组
 * 3. 对于当前元素，将栈中所有小于等于当前元素的元素弹出
 * 4. 栈顶元素（如果存在）就是当前元素右侧第一个更大的元素
 * 5. 将当前元素下标压入栈中
 * 
 * 时间复杂度分析：
 * - 每个元素最多入栈和出栈各一次
 * - 总时间复杂度为 O(n)
 */
class ExtMonotonicStack {
public:
    /**
     * 寻找每个位置右侧第一个更大元素的下标
     * @param nums 输入数组
     * @return 结果数组，result[i] 表示 nums[i] 右侧第一个更大元素的下标，不存在则为 -1
     */
    static std::vector<int> nextGreaterElements(const std::vector<int>& nums) {
        int n = nums.size();
        std::vector<int> result(n, -1);
        std::stack<int> stk;  // 单调递减栈，存储数组下标
        
        // 从右到左遍历数组
        for (int i = n - 1; i >= 0; i--) {
            // 将栈中所有小于等于当前元素的元素弹出
            while (!stk.empty() && nums[stk.top()] <= nums[i]) {
                stk.pop();
            }
            
            // 栈顶元素就是右侧第一个更大的元素
            if (!stk.empty()) {
                result[i] = stk.top();
            }
            
            // 将当前元素下标压入栈中
            stk.push(i);
        }
        
        return result;
    }
    
    /**
     * 验证结果的正确性（暴力解法，用于测试）
     * 时间复杂度：O(n^2)
     */
    static std::vector<int> nextGreaterElementsBruteForce(const std::vector<int>& nums) {
        int n = nums.size();
        std::vector<int> result(n, -1);
        
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (nums[j] > nums[i]) {
                    result[i] = j;
                    break;
                }
            }
        }
        
        return result;
    }
    
    /**
     * 打印数组和结果
     */
    static void printResult(const std::vector<int>& nums, const std::vector<int>& result) {
        std::cout << "Array: [";
        for (int i = 0; i < nums.size(); i++) {
            std::cout << nums[i];
            if (i < nums.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
        
        std::cout << "Result: [";
        for (int i = 0; i < result.size(); i++) {
            std::cout << result[i];
            if (i < result.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
};

#endif // EXTMONOTONICSTACK_HPP