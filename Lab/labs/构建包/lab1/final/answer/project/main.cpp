#include <iostream>
#include <fstream>
#include "include/MergeSort.hpp"
#include "include/InsertionSort.hpp"
#include "include/dessert.hpp"
#include "include/MyStack.hpp"
#include "include/SqList_Student.hpp"
#include "include/Linklist_Student.hpp"

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

desserts("Test of SqList"){
	string sample_input_file = "./data/samples/data_SqList.in";
	ifstream ifs(sample_input_file);
	if(!ifs.is_open()){
		cout << "open input_file failed" << endl;
		return;
	}
	int test_num;
	ifs >> test_num;
	for(int i = 0; i < test_num; i++){
    	SqList list;
    	int n, m, num;
		ifs >> n;
    	for(int j = 0; j < n; j++){
			ifs >> num;
    		list.push_back(num);
		}
		auto origin_list = list;
		ifs >> m;
		for(int i = 0; i < m; i++){
			ifs >> num;
			origin_list.push_back(num);
			insert(list, num);
		}
		sort(origin_list.begin(), origin_list.end());
		dessert((origin_list == list));
	}
}

desserts("Test of Linklist"){
	string sample_input_file = "./data/samples/data_Linklist.in";
	ifstream ifs(sample_input_file);
	if(!ifs.is_open()){
		cout << "open input_file failed" << endl;
		return;
	}
	int test_num;
	ifs >> test_num;
	for(int i = 0; i < test_num; i++){
		int n, k = 0;
		bool judge = true;
		vector<int> temp;
		Linklist head, tail, point;
    	head = tail = new node();
		ifs >> n;
    	for(int i = 0; i < n; i++){
        	point = new node();
			ifs >> point->num;
			temp.push_back(point->num);
        	tail->next = point;
        	point->prior = tail;
        	tail = point;
    	}
    	head->prior = tail;
    	tail->next = head;
    	changeList(head);

    	point = head->next;
		for(int i = 0; i < n; i = i + 2){
			if(temp[i] != point->num){
				judge = false;
			}
			point = point->next;
		}
		for(int i = n % 2 == 0 ? n - 1 : n - 2; i > -1; i = i - 2){
			if(temp[i] != point->num){
				judge = false;
			}
			point = point->next;
		}
		dessert(judge);
	}
}

desserts ("Test of InsertionSort (samples)") {
	string sample_input_file = "./data/samples/data_sort.in";
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
	string test_input_file = "./data/test/data_sort.in";
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
	string sample_input_file = "./data/samples/data_sort.in";
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
	string test_input_file = "./data/test/data_sort.in";
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