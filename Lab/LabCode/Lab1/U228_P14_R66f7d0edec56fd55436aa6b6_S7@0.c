#include <vector>
using namespace std;

int merge(vector<int>& nums, int left, int mid, int right, vector<int>& temp) {
    int i = left, j = mid + 1, k = 0, inv_count = 0;
    int total_inv_count = 0;

    while (i <= mid && j <= right) {
        if (nums[i] <= nums[j]) {
            temp[k++] = nums[i++];
        }
        else {
            temp[k++] = nums[j++];
            inv_count = mid - i + 1; 
        }
    }

    while (i <= mid) {
        temp[k++] = nums[i++];
    }
    while (j <= right) {
        temp[k++] = nums[j++];
    }

    for (i = left; i <= right; ++i) {
        nums[i] = temp[i - left];
    }
    return inv_count;
}

int mergeSort(vector<int>& nums, int left, int right, vector<int>& temp) {
    int mid, inv_count = 0;
    if (right > left) {
        mid = left + (right - left) / 2;
        inv_count += mergeSort(nums, left, mid, temp);
        inv_count += mergeSort(nums, mid + 1, right, temp);
        inv_count += merge(nums, left, mid, right, temp);
    }
    return inv_count;
}

int reversePairs(vector<int>& nums) {
    int n = nums.size();
    vector<int> temp(n);
    return mergeSort(nums, 0, n - 1, temp);
}

int main() {
    int n;
    cin >> n;
    vector<int> nums(n);
    for (int i = 0; i < n; ++i) {
        cin >> nums[i];
    }

    cout << reversePairs(nums) << endl;
    return 0;
}