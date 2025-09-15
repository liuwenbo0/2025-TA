#include <iostream>
#include <vector>
void merge(std::vector<int>& arr,int left,int mid, long& compCount){
    std::vector<int> temp(right-left+1);
    int i=left;
    int j=mid+1;
    int k=0;

    while(i<=mid && j<=right){
        if(arr[i]<=arr[j]){
            temp[k++] = arr[i++];

        }
        else{
            temp[k++]=arr[j++];
            compCount +=(mid-i+1);
        }
    }
    while(i<=mid)
    temp[k++]=arr[i++];
    while(j<=right)
    temp[k++]=arr[j++];
    for(int i = left;i<=right;++i)
    arr[i]=temp[i-left];
}
void mergeSort(std::vector<int>& arr, int left, int right, long& compCount){
    if(left >= right) return;
    int mid=left=(right-left)/2;
    mergeSort(arr,left,mid,compCount);
    mergeSort(arr,mid+1,right,compCount);
    merge(arr,left,mid,right,compCount);
}
void insertionSort(std::vector<int<& arr, long& compCount){
    for(size_t i=1;i<arr.size();++i){
    int key = arr[i];
    int j = i-1;
    while(j>=0&& arr[j]>key){
        arr[j+1]=arr[j];
        --j;
        compCount++;
    }
    arr[j+1]=key;
    }
    int main(){
        int N;
        std::cin>>N;
        std::vector<int>arr(N);
        for(int i = 0;i<N;i++){
            std::cin>>arr[i];
        }
        long mergeCompCount=0;
        long insertCompCount=0;
        mergeSort(arr,0,N-1,mergeCompCount);
        std::vector<int> originalArr = arr;
        insertionSort(originalArr,insertCompCount);
        return 0;
    }
}