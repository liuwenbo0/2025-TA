#include "MergeSort.h"
    
// 通过归并排序对int队列nums中的[left, right]区间进行升序排序
// @param
// nums: 完整的待排序队列，最终排序的结果应存放在nums中
// left: 当前排序区间的左端点
// right: 当前排序区间的右端点
void merge(std::vector<int> &nums, int left, int mid, int right)
{
   int n1 = mid - left + 1;
   int n2 = right - mid;
   std::vector<int>leftArr(n1);
   std::vector<int>rightArr(n2);
   for(int i=0;i<n1;++i)
   leftArr[i]=nums[left+i];
   for(int i= 0;i<n2;++i)
   rightArr[i]=nums[mid+1+i];
   int i=0,j=0,k=left;
   while(i<n1 && j<n2)
   {
    if(leftArr[i]<=rightArr[j]){
        nums[k]=leftArr[i];
        ++i;
    }
    else
    {
        nums[k]=rightArr[j];
        ++j;
    }
    ++k;
   }
   while(i<n1)
   {
    nums[k]=leftArr[i];
    ++i;
    ++k;
   }
   while(j<n2)
   {
    nums[k]=rightArr[j];
    ++j;
    ++k;
   }

}

void MergeSort::merge_sort_aux(std::vector<int> &nums, int left, int right)
{
    if(left<right)
    {
        int mid=left+(right-left)/2;
        merge_sort_aux(nums,left,mid);
        merge_sort_aux(nums,mid+1,right);
        merge(nums,left,mid,right);
    }
}