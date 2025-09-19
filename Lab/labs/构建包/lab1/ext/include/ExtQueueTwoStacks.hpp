#ifndef EXTQUEUETWOSTACKS_HPP
#define EXTQUEUETWOSTACKS_HPP

#include <stack>
#include <stdexcept>

/**
 * 拓展题1：用两个栈实现队列（摊还 O(1)）
 * 
 * 要求：只使用两个栈支持 push(x)、pop()、front()，并说明摊还时间复杂度为何为 O(1)。
 * 
 * 摊还分析：
 * - push 操作：直接压入 stack1，时间复杂度 O(1)
 * - pop/front 操作：
 *   - 如果 stack2 非空，直接操作 stack2 顶部，时间复杂度 O(1)
 *   - 如果 stack2 为空，将 stack1 所有元素转移到 stack2，然后操作
 *   - 虽然单次转移操作可能是 O(n)，但每个元素最多被转移一次
 *   - 因此摊还时间复杂度为 O(1)
 */
class ExtQueueTwoStacks {
private:
    std::stack<int> stack1;  // 用于 push 操作
    std::stack<int> stack2;  // 用于 pop 和 front 操作
    
    // 将 stack1 的所有元素转移到 stack2
    void transfer() {
        while (!stack1.empty()) {
            stack2.push(stack1.top());
            stack1.pop();
        }
    }
    
public:
    /**
     * 向队列尾部添加元素
     * 时间复杂度：O(1)
     */
    void push(int x) {
        stack1.push(x);
    }
    
    /**
     * 从队列头部移除元素
     * 摊还时间复杂度：O(1)
     */
    void pop() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        
        if (stack2.empty()) {
            transfer();
        }
        stack2.pop();
    }
    
    /**
     * 获取队列头部元素
     * 摊还时间复杂度：O(1)
     */
    int front() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        
        if (stack2.empty()) {
            transfer();
        }
        return stack2.top();
    }
    
    /**
     * 检查队列是否为空
     * 时间复杂度：O(1)
     */
    bool empty() const {
        return stack1.empty() && stack2.empty();
    }
    
    /**
     * 获取队列大小
     * 时间复杂度：O(1)
     */
    size_t size() const {
        return stack1.size() + stack2.size();
    }
    
    /**
     * 清空队列
     */
    void clear() {
        while (!stack1.empty()) stack1.pop();
        while (!stack2.empty()) stack2.pop();
    }
};

#endif // EXTQUEUETWOSTACKS_HPP