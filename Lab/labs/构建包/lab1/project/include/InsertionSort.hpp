#pragma once
#include "MySort.hpp"

class InsertionSort: public MySort {
public:
  // 通过插入排序对int队列nums进行升序排序
  // @param
  // nums: 完整的待排序队列，最终排序的结果应存放在nums中
  void mysort(std::vector<int>& nums) {
    //请在这里完成你的代码
    for (int i = 1; i < nums.size(); ++i) {
        int x = nums[i];
        int j = i - 1;
        while (j >= 0 && nums[j] > x) {
            nums[j + 1] = nums[j];
            --j;
        }

        nums[j + 1] = x;
    }
    
  }
private:
  int cnt;
};