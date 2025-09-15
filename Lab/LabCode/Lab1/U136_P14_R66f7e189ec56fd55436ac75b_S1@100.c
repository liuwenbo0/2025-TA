#include <stdio.h>
#include <stdlib.h>
long long merge_and_count(int *arr, int *temp_arr, int left, int right) {
    long long count = 0;
    int mid = (left + right) / 2;
    if (left >= right) {
        return 0;
    }
    count += merge_and_count(arr, temp_arr, left, mid);
    count += merge_and_count(arr, temp_arr, mid + 1, right);
    int i = left;
    int j = mid + 1;
    int k = left;
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp_arr[k] = arr[i];
            i++;
        } else {
            temp_arr[k] = arr[j];
            count += (mid - i + 1);
            j++;
        }
        k++;
    }
    while (i <= mid) {
        temp_arr[k] = arr[i];
        i++;
        k++;
    }
    while (j <= right) {
        temp_arr[k] = arr[j];
        j++;
        k++;
    }
    for (int i = left; i <= right; i++) {
        arr[i] = temp_arr[i];
    }
    return count;
}
int main() {
    int n;
    scanf("%d", &n);
    int *arr = (int *) malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }
    int *temp_arr = (int *) malloc(n * sizeof(int));
    long long inv_count = merge_and_count(arr, temp_arr, 0, n - 1);
    printf("%lld\n", inv_count);
    free(arr);
    free(temp_arr);
    return 0;
}