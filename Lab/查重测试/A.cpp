#include "MergeSort.h"
    
// 通过归并排序对int队列nums中的[left, right]区间进行升序排序
// @param
// nums: 完整的待排序队列，最终排序的结果应存放在nums中
// left: 当前排序区间的左端点
// right: 当前排序区间的右端点
void merge(std::vector<int> &nums, int left, int mid, int right){
    int cur1=left,cur2=mid+1;
    vector<int> temp;
    while(cur1<=mid&&cur2<=right){
        if(nums[cur1]<nums[cur2]){
            temp.push_back(nums[cur1]);
            cur1++;
        }
        else{
            temp.push_back(nums[cur2]);
            cur2++;
        }
    }
    while(cur1<=mid){
        temp.push_back(nums[cur1]);
        cur1++;
    }
    while(cur2<=right){
        temp.push_back(nums[cur2]);
        cur2++;
    }
    for(int i=0;i<temp.size();i++){
        nums[left+i]=temp[i];
    }
    return;
}
void MergeSort::merge_sort_aux(std::vector<int> &nums, int left, int right)
{
    if(left<right){
        int mid=(left+right)/2;
        merge_sort_aux(nums,left,mid);
        merge_sort_aux(nums,mid+1,right);
        merge(nums,left,mid,right);
    }
    return;
}