#include <stdio.h>

int merge_and_count(int arr[], int temp[], int left, int mid, int right) {
    int i = left;    // 左子数组的起始索引
    int j = mid + 1; // 右子数组的起始索引
    int k = left;    // 合并后的数组的起始索引
    int inv_count = 0;

    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            // 有 (mid - i + 1) 个逆序对
            temp[k++] = arr[j++];
            inv_count += (mid - i + 1);
        }
    }

    while (i <= mid) {
        temp[k++] = arr[i++];
    }

    while (j <= right) {
        temp[k++] = arr[j++];
    }

    for (i = left; i <= right; i++) {
        arr[i] = temp[i];
    }

    return inv_count;
}

int merge_sort_and_count(int arr[], int temp[], int left, int right) {
    int inv_count = 0;
    if (left < right) {
        int mid = (left + right) / 2;
        inv_count += merge_sort_and_count(arr, temp, left, mid);
        inv_count += merge_sort_and_count(arr, temp, mid + 1, right);
        inv_count += merge_and_count(arr, temp, left, mid, right);
    }
    return inv_count;
}

int count_inversion_pairs(int arr[], int n) {
    int temp[n];
    return merge_sort_and_count(arr, temp, 0, n - 1);
}

int main() {
    int n;
    scanf("%d", &n);
    int arr[n];
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }
    
    int result = count_inversion_pairs(arr, n);
    printf("%d\n", result);
    return 0;
}