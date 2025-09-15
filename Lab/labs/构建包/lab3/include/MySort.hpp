#pragma once

#include "util.hpp"
using namespace std;

class MySort {
public:
	// 样例数据文件所在位置
	std::string sample_input_file = "./data/samples/data.in";
	// 控制台输出颜色
	std::string RED = "\033[1;31m";
	std::string GREEN = "\033[1;32m";
	std::string END = "\033[0m";

public:
	// 用于测试排序算法的正确性
	void test_sort() {
		std::ifstream ifs(sample_input_file);
		if (!ifs.is_open()) {
			std::cout << "open input_file failed" << std::endl;
			return;
		}
		
		// data.in 中第一行为测试数据组数
		// 每一组测试数据，第一行为数组长度，第二行为数组元素
		
		// test_groups 为测试组数，succ_cnt 为测试成功的组数
		int test_groups = 0, succ_cnt = 0;
		// 从文件中读入测试数据组数
		ifs >> test_groups;
		std::cout << "Total number of sample tests: "<< test_groups << std::endl;
		
		// 循环每组测试数据
		for (int t = 1; t <= test_groups; t++) {
			int n = 0;
			// 读入数组长度以及数组
			ifs >> n;
			std::vector<int> nums(n);
			for(int i = 0; i < n; i++){
				ifs >> nums[i];
			}
			
			auto ans = nums;
			std::sort(ans.begin(), ans.end());
			mysort(nums);

			if(compare_result("Sample Test Round " + to_string(t), nums, ans)) {
      succ_cnt ++;
    }
		}
		if(succ_cnt != test_groups) {
			std::cout << "The percentage of successful samples: " << succ_cnt << "/" << test_groups << std::endl;
		} else {
			std::cout << GREEN << "Congratulation, you have passed all test cases!" << END << std::endl;
		}
	}
	
	// 纯虚函数，需要在子类中实现
	virtual void mysort(std::vector<int>& nums) = 0;
};