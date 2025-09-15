#if __has_include("util.hpp")
#include "util.hpp"
#endif

class Solution {
public:

  vector<vector<int>> round_robin_schedule(int n) {
    // 如此构造一个二维数组（ nxn 矩阵）
    // ans[i][j]表示矩阵的第i行第j列
    vector<vector<int> > ans;
    for(int i=0; i<n; i++) {
      ans.push_back(vector<int>(n, 0));
    }
    // 请在这里完成你的代码

    return ans;
  }
};