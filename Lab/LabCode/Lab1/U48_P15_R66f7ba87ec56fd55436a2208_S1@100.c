#include <stdio.h>

int insertion_sort(int arr[], int n) {
    int comparisons = 0;
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {  // 比较操作
            comparisons++;
            arr[j + 1] = arr[j];
            j--;
        }
        if (j >= 0) {  // 如果此时 arr[j] <= key, 也需要计数
            comparisons++;
        }
        arr[j + 1] = key;
    }
    return comparisons;
}

void merge(int arr[], int left, int mid, int right, int *comparisons) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int L[n1], R[n2];

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        (*comparisons)++;  // 比较操作
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void merge_sort(int arr[], int left, int right, int *comparisons) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(arr, left, mid, comparisons);
        merge_sort(arr, mid + 1, right, comparisons);
        merge(arr, left, mid, right, comparisons);
    }
}

int main() {
    int N;
    scanf("%d", &N);
    int arr[N];

    for (int i = 0; i < N; i++) {
        scanf("%d", &arr[i]);
    }

    // 复制数组用于两个排序
    int arr_for_insertion[N];
    int arr_for_merge[N];
    for (int i = 0; i < N; i++) {
        arr_for_insertion[i] = arr[i];
        arr_for_merge[i] = arr[i];
    }

    int insertion_comparisons = insertion_sort(arr_for_insertion, N);
    int merge_comparisons = 0;
    merge_sort(arr_for_merge, 0, N - 1, &merge_comparisons);

    printf("%d %d\n", merge_comparisons, insertion_comparisons);
    return 0;
}