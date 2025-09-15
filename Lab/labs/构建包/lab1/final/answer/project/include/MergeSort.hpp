#pragma once
#include "MySort.hpp"

class MergeSort : public MySort
{
public:
  // 通过归并排序对int队列nums中的[left, right]区间进行升序排序
  // @param
  // nums: 完整的待排序队列，最终排序的结果应存放在nums中
  // left: 当前排序区间的左端点
  // right: 当前排序区间的右端点
  void merge_sort_aux(std::vector<int> &nums, int left, int right)
  {
    int mid_index;
    int i, j, length;
    if(left < right){
      mid_index = (left + right) / 2;
      merge_sort_aux(nums, left, mid_index);
      merge_sort_aux(nums, mid_index + 1, right);
      length = right - left + 1;
      std::vector<int> temp(length);
      i = left;
      j = mid_index + 1;
      for(int k  = 0; k < length; k++){
        if(i < mid_index + 1 && j < right + 1){
          if(nums[i] < nums[j]){
            temp[k] = nums[i++];
          }else{
            temp[k] = nums[j++];
          }
        }else if(i > mid_index){
          temp[k] = nums[j++];
        }else if(j > right){
          temp[k] = nums[i++];
        }
      }
      for(int k = 0; k < length; k++){
        nums[left + k] = temp[k];
      }
    }
  }

  void mysort(std::vector<int> &nums)
  {
    int n = nums.size();
    merge_sort_aux(nums, 0, n - 1);
  }
};