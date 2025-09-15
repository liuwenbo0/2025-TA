#include "Matrix.h"




void MatrixMult::mult() {

    const int BLOCK_SIZE = 32;


    if (p * q * r > 50000) {
        vector<vector<int>> B_T;

        B_T.resize(r);
        for (auto &x : B_T) { x.resize(q); }

        for (int i = 0; i < q; ++i) {
            for (int j = 0; j < r; ++j) {
                B_T[j][i] = B[i][j];
            }
        }

        for (int i = 0; i < p; ++i) {
            for (int j = 0; j < r; ++j) {
                C[i][j] = 0;
            }
        }

        #pragma omp parallel for num_threads(4)
        for (int i = 0; i < p; i += BLOCK_SIZE) {
            for (int j = 0; j < r; j += BLOCK_SIZE) {
                for (int k = 0; k < q; k += BLOCK_SIZE) {

                    for (int ii = i; ii < min(i + BLOCK_SIZE, p); ++ii) {
                        for (int jj = j; jj < min(r, j + BLOCK_SIZE); ++jj) {
                            int c = C[ii][jj];
                            for (int kk = k; kk < min(q, k + BLOCK_SIZE); ++kk) {
                                c += A[ii][kk] * B_T[jj][kk];
                            }
                            C[ii][jj] = c;
                        }
                    }
                }
            }
        }
    } else {
        int c = 0;
        for (int i = 0; i < p; ++i) {
            for (int j = 0; j < r; ++j) {
                c = 0;
                for (int k = 0; k < q; ++k) {
                    c += A[i][k] * B[k][j];
                }
                C[i][j] = c;
            }
        }
    }
}