#include <iostream>
#include <fstream>
#include "include/MergeSort.hpp"
#include "include/InsertionSort.hpp"
#include "include/dessert.hpp"
#include "include/MyStack.hpp"

using namespace std;

desserts ("Test of MyStack") {
	MyStack stack = MyStack();
	stack.Initstack();
	dessert(stack.Push(1));
	dessert(!stack.Full());
	dessert(!stack.Empty());
	ElementType e = -1;
	dessert(stack.Pop(e) && e == 1);
	dessert(stack.Empty());
	dessert(!stack.Full());
	bool big_insertion = true;
	for (int i=1; i<=100; i++) {
		big_insertion &= stack.Push(i);
	}
	dessert(big_insertion);
	dessert(stack.Full());
	dessert(!stack.Push(101));
	bool stack_lifo = true;
	for (int i=100; i>=1; i--) {
		ElementType e;
		stack.Pop(e);
		stack_lifo &= (e == i);
	}
	dessert(stack_lifo);
}

desserts ("Test of InsertionSort (samples)") {
	string sample_input_file = "./data/samples/data.in";
	ifstream ifs(sample_input_file);
	if (!ifs.is_open()) {
		cout << "open input_file failed" << std::endl;
		return;
	}
	
	int test_groups = 0, succ_cnt = 0;
	ifs >> test_groups;
	
	for (int t = 1; t <= test_groups; t++) {
		int n = 0;
		ifs >> n;
		vector<int> nums(n);
		for(int i = 0; i < n; i++){
			ifs >> nums[i];
		}
		
		auto origin_nums = nums;
		sort(origin_nums.begin(), origin_nums.end());

		InsertionSort insertionsort = InsertionSort();
		insertionsort.mysort(nums);
		dessert((origin_nums == nums));
	}
}

desserts ("Test of InsertionSort (performance)") {
	string test_input_file = "./data/test/data.in";
	ifstream ifs(test_input_file);
	if (!ifs.is_open()) {
		cout << "open input_file failed" << std::endl;
		return;
	}
	
	int test_groups = 0, succ_cnt = 0;
	ifs >> test_groups;
	
	for (int t = 1; t <= test_groups; t++) {
		int n = 0;
		ifs >> n;
		vector<int> nums(n);
		for(int i = 0; i < n; i++){
			ifs >> nums[i];
		}
		
		auto origin_nums = nums;
		sort(origin_nums.begin(), origin_nums.end());

		InsertionSort insertionsort = InsertionSort();
		insertionsort.mysort(nums);
		dessert((origin_nums == nums));
	}
}
desserts ("Test of MergeSort (samples)") {
	string sample_input_file = "./data/samples/data.in";
	ifstream ifs(sample_input_file);
	if (!ifs.is_open()) {
		cout << "open input_file failed" << std::endl;
		return;
	}
	
	int test_groups = 0, succ_cnt = 0;
	ifs >> test_groups;
	
	for (int t = 1; t <= test_groups; t++) {
		int n = 0;
		ifs >> n;
		vector<int> nums(n);
		for(int i = 0; i < n; i++){
			ifs >> nums[i];
		}
		
		auto origin_nums = nums;
		sort(origin_nums.begin(), origin_nums.end());

		MergeSort mergesort = MergeSort();
		mergesort.mysort(nums);
		dessert((origin_nums == nums));
	}
}

desserts ("Test of MergeSort (performance)") {
	string test_input_file = "./data/test/data.in";
	ifstream ifs(test_input_file);
	if (!ifs.is_open()) {
		cout << "open input_file failed: " << test_input_file << endl;
		return;
	}
	
	int test_groups = 0, succ_cnt = 0;
	ifs >> test_groups;
	
	for (int t = 1; t <= test_groups; t++) {
		int n = 0;
		ifs >> n;
		vector<int> nums(n);
		for(int i = 0; i < n; i++){
			ifs >> nums[i];
		}
		
		auto origin_nums = nums;
		sort(origin_nums.begin(), origin_nums.end());

		MergeSort mergesort = MergeSort();
		mergesort.mysort(nums);
		dessert((origin_nums == nums));
	}
}
int main(){
	// Nothing to do...
	return 0;
}