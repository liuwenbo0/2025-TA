#include <iostream>
#include "include/HeapSort.hpp"
#include "include/QuickSort.hpp"
#include "include/ThreeWayQuickSort.hpp"

using namespace std;

int main(){
	HeapSort my_heap_sort;
	QuickSort my_quick_sort;
	ThreeWayQuickSort my_three_way_quick_sort;

  	bool loop = true;
	while (loop) {
		cout << endl;
		cout << "Select a Sort Algorithm to Test Run: " << endl;
		cout << "1.Heap Sort" << endl;
		cout << "2.Quick Sort" << endl;
		cout << "3.Three Way Quick Sort" << endl;
		cout << "others to exit" << endl;
		int opt = 0;
		cin >> opt;
		switch (opt) {
		case 1:
			my_heap_sort.test_sort();
			break;
		case 2:
			my_quick_sort.test_sort();
			break;
		case 3:
			my_three_way_quick_sort.test_sort();
			break;
		default:
			loop = false;
			break;
		}
		getchar();
		if(opt < 1 || opt > 5) break;
		do {
			cout << "Please enter Enter to continue";
			if(getchar() == 10) break;	
		} while(true);
	}
  return 0;
}