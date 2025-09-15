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
    //请在这里完成你的代码
  }

  void mysort(std::vector<int> &nums)
  {
    int n = nums.size();
    merge_sort_aux(nums, 0, n - 1);
  }
};