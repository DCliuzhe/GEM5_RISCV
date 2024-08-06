#include <stdint.h>
#include <stdio.h>


// GEMM: C = A * B, where A, B, C are N x N matrices
void gemm(int N, float A[N][N], float B[N][N], float C[N][N]) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {

    const int N = 128;  // 矩阵大小
    float A[N][N], B[N][N], C[N][N];

    // 初始化矩阵 A 和 B
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i][j] = 1;
            B[i][j] = 1;
        }
    }

    // 执行 GEMM 操作
    gemm(N, A, B, C);

    // 打印矩阵 C 的结果
    printf("Matrix C:\n");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            printf("%f ", C[i][j]);
        }
        printf("\n");
    }

    return 0;
}

