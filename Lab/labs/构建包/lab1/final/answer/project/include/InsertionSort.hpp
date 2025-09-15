#pragma once
#include "MySort.hpp"

class InsertionSort: public MySort {
public:
  // 通过插入排序对int队列nums进行升序排序
  // @param
  // nums: 完整的待排序队列，最终排序的结果应存放在nums中
  void mysort(std::vector<int>& nums) {
    int length = nums.size();
    int min_index, temp;
    for(int i = 0; i < length; i++){
      min_index = i;
      for(int j = i; j < length; j++){
        if(nums[min_index] > nums[j]){
          min_index = j;
        }
      }
      temp = nums[i];
      nums[i] = nums[min_index];
      nums[min_index] = temp;;
    }
  }
private:
  int cnt;
};