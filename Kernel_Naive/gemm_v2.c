#include <stdint.h>
#include <stdio.h>

void gemm(double *a, double *b, double *c, int m, int n, int k)
{
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
        {
            c[i * n + j] = 0;
            for (int q = 0; q < k; ++q) 
                c[i * n + j] += a[i * k + q] * b[q * n + j];
        }
}


int main() {

    const int m = 48;
    const int n = 32;
    const int k = 64;

    double A[m * k];
    double B[k * n];
    double C[m * n];

    // 初始化矩阵 A 
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < k; ++j)
            A[i * k + j] = 1;

    for (int i = 0; i < k; ++i)
        for (int j = 0; j < n; ++j)
            B[i * n + j] = 1;

    // 执行 GEMM 操作
    gemm(A, B, C, m, n, k);

    // 打印矩阵 C 的结果
    printf("Matrix C:\n");
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%f ", C[i * n + j]);
        }
        printf("\n");
    }

    return 0;
}
