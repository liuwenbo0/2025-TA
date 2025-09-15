#if __has_include("util.hpp")
#include "util.hpp"
#endif

/*
提示：
int范围：
  无穷小可以用 INT_MIN 表示
  无穷大可以用 INT_MAX 表示
long long 范围：
  无穷小可以用 LLONG_MIN 表示
  无穷大可以用 LLONG_MAX 表示
*/

class Solution {
public:
  int find_maximum_crossing_subarray(vector<int> &A, int low, int mid, int high) {
    // 请在这里完成你的代码

    return 0;
  }
  int find_maximum_subarray(vector<int> &A, int low, int high) {
    // 请在这里完成你的代码

    return 0;
  }
  int find_maximum_subarray(vector<int> &A) {
    return find_maximum_subarray(A, 0, A.size() - 1);
  }
};