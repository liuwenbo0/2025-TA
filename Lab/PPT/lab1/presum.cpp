#include <bits/stdc++.h>
using namespace std;

void print_vec(const auto &v) {
    for (const auto &x : v) {
        cout << x << " ";
    }
    cout << endl;
}

int main() {
    vector<int> A = {2, 1, 3, 1, 1, 3, 2, 0};
    vector<int> B(A.size()), C(A.size());
    partial_sum(A.begin(), A.end(), B.begin());
    adjacent_difference(A.begin(), A.end(), C.begin());

    print_vec(B);
    print_vec(C);

}