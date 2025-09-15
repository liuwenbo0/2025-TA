#include <stdio.h>
#include <stdlib.h>

// 合并两个子数组，并更新逆序对计数
void merge(int arr[], int temp[], int left, int mid, int right, long long *inv_count) {
    int i, j, k;
    i = left;
    j = mid + 1;
    k = left;
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
            // 左侧子数组中所有剩余元素都与右侧子数组的当前元素形成逆序对
            *inv_count += (mid - i + 1);
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
}

// 递归地对数组进行归并排序，并在合并过程中更新逆序对计数
void mergeSort(int arr[], int temp[], int left, int right, long long *inv_count) {
    if (right > left) {
        int mid = (right + left) / 2;
        mergeSort(arr, temp, left, mid, inv_count);
        mergeSort(arr, temp, mid + 1, right, inv_count);
        merge(arr, temp, left, mid, right, inv_count);
    }
}

int main() {
    int n;
    scanf("%d", &n);
    int *arr = (int *)malloc(n * sizeof(int));
    int *temp = (int *)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    long long inv_count = 0;
    mergeSort(arr, temp, 0, n - 1, &inv_count);

    printf("%lld\n", inv_count);
    free(arr);
    free(temp);
    return 0;
}