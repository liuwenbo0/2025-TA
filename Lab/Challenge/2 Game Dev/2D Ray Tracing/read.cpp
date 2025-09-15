#include "bits/stdc++.h"
using namespace std;

int main() {
    // auto ret = freopen("0.in", "r", stdin);
    // cout << "ret: " << ret << endl;

    ifstream inFile("1.in");
    if (!inFile) {
        cerr << "Failed to open input file!" << endl;
        return 1;
    }

    int N;
    inFile >> N;
    cout << "N: " << N << endl;
}