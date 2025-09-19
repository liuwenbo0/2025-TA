#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <filesystem>

using namespace std;

// 生成队列测试数据
void generateQueueData(int testCase) {
    string dir = "data/queue/";
    string inputFile = dir + "input" + to_string(testCase) + ".txt";
    string outputFile = dir + "output" + to_string(testCase) + ".txt";
    
    ofstream input(inputFile);
    ofstream output(outputFile);
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> valueDis(1, 1000);
    uniform_int_distribution<> opDis(1, 3);  // 1: push, 2: pop, 3: front
    
    int numOps = 50 + testCase * 10;  // 测试用例越后面，操作越多
    vector<int> expectedQueue;  // 模拟队列状态
    
    input << numOps << endl;
    
    for (int i = 0; i < numOps; i++) {
        int op = opDis(gen);
        
        // 确保队列不为空时才能进行 pop 和 front 操作
        if (expectedQueue.empty() && (op == 2 || op == 3)) {
            op = 1;  // 强制变为 push
        }
        
        if (op == 1) {  // push
            int value = valueDis(gen);
            input << "push " << value << endl;
            expectedQueue.push_back(value);
        }
        else if (op == 2) {  // pop
            input << "pop" << endl;
            if (!expectedQueue.empty()) {
                expectedQueue.erase(expectedQueue.begin());
                output << "pop success" << endl;
            }
        }
        else if (op == 3) {  // front
            input << "front" << endl;
            if (!expectedQueue.empty()) {
                output << expectedQueue.front() << endl;
            }
        }
    }
    
    input.close();
    output.close();
    
    cout << "生成队列测试用例 " << testCase << ": " << numOps << " 个操作" << endl;
}

// 生成单调栈测试数据
void generateMonoStackData(int testCase) {
    string dir = "data/monostack/";
    string inputFile = dir + "input" + to_string(testCase) + ".txt";
    string outputFile = dir + "output" + to_string(testCase) + ".txt";
    
    ofstream input(inputFile);
    ofstream output(outputFile);
    
    random_device rd;
    mt19937 gen(rd());
    
    int n = 10 + testCase * 5;  // 数组大小
    uniform_int_distribution<> valueDis(1, 100);
    
    vector<int> nums(n);
    for (int i = 0; i < n; i++) {
        nums[i] = valueDis(gen);
    }
    
    // 写入输入文件
    input << n << endl;
    for (int i = 0; i < n; i++) {
        input << nums[i];
        if (i < n - 1) input << " ";
    }
    input << endl;
    
    // 计算正确答案（暴力解法）
    vector<int> result(n, -1);
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (nums[j] > nums[i]) {
                result[i] = j;
                break;
            }
        }
    }
    
    // 写入输出文件
    for (int i = 0; i < n; i++) {
        output << result[i];
        if (i < n - 1) output << " ";
    }
    output << endl;
    
    input.close();
    output.close();
    
    cout << "生成单调栈测试用例 " << testCase << ": 数组大小 " << n << endl;
}

int main() {
    cout << "=== 生成拓展题测试数据 ===" << endl;
    
    // 创建数据目录（如果不存在）
    filesystem::create_directories("data/queue");
    filesystem::create_directories("data/monostack");
    
    // 生成10组队列测试数据
    cout << "\n生成队列测试数据：" << endl;
    for (int i = 1; i <= 10; i++) {
        generateQueueData(i);
    }
    
    // 生成10组单调栈测试数据
    cout << "\n生成单调栈测试数据：" << endl;
    for (int i = 1; i <= 10; i++) {
        generateMonoStackData(i);
    }
    
    cout << "\n所有测试数据生成完成！" << endl;
    return 0;
}