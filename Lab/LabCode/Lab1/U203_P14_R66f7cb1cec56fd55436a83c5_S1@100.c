#include <stdio.h>  
#include <stdlib.h>  
  
long long mergeCountSplitInv(int arr[], int temp[], int left, int mid, int right) {  
    int i, j, k;  
    long long inv_count = 0;  
  
    i = left; // 初始索引第一个子数组  
    j = mid;  // 初始索引第二个子数组  
    k = left; // 初始索引临时数组  
  
    while ((i <= mid - 1) && (j <= right)) {  
        if (arr[i] <= arr[j]) {  
            temp[k++] = arr[i++];  
        } else {  
            temp[k++] = arr[j++];  
            // 因为arr[i]大于arr[j]，并且arr[j...right]中的每个元素都大于arr[i]，  
            // 所以逆序对数量需要增加mid - i + 1  
            inv_count += (mid - i);  
        }  
    }  
  
    // 复制r[i..mid]剩余的元素  
    while (i <= mid - 1)  
        temp[k++] = arr[i++];  
  
    // 复制r[j..right]剩余的元素  
    while (j <= right)  
        temp[k++] = arr[j++];  
  
    // 将temp[]中的元素复制回arr[]，以便arr[]现在包含排序后的子数组  
    for (i = left; i <= right; i++)  
        arr[i] = temp[i];  
  
    return inv_count;  
}  
  
// 主要的归并排序函数，计算逆序对  
long long mergeSortAndCountInv(int arr[], int temp[], int left, int right) {  
    long long inv_count = 0;  
    int mid;  
  
    if (right > left) {  
        // 找到中间点  
        mid = (right + left) / 2;  
  
        // 递归计算左半部分和右半部分的逆序对  
        inv_count += mergeSortAndCountInv(arr, temp, left, mid);  
        inv_count += mergeSortAndCountInv(arr, temp, mid + 1, right);  
  
        // 合并两半并计算跨越两部分的逆序对  
        inv_count += mergeCountSplitInv(arr, temp, left, mid + 1, right);  
    }  
  
    return inv_count;  
}  
  
// 外部调用的包装函数  
long long countInversions(int arr[], int n) {  
    int *temp = (int *)malloc(n * sizeof(int));  
    long long inv_count = mergeSortAndCountInv(arr, temp, 0, n - 1);  
    free(temp);  
    return inv_count;  
}  
  
int main() {  
    int n;  
    scanf("%d", &n);  
    int arr[n];  
  
    for (int i = 0; i < n; i++) {  
        scanf("%d", &arr[i]);  
    }  
  
    long long inv_count = countInversions(arr, n);  
    printf("%lld\n", inv_count);  
  
    return 0;  
}