#include <stdint.h>
#include <stdio.h>

// 定义数据类型，可以根据需要更改为 float, double, int8_t, int16_t 等
typedef int8_t DataType;

// 通用的 GEMM 函数，使用 typedef 定义的数据类型
void gemm(DataType *a, DataType *b, DataType *c, int m, int n, int k) {
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j) {
            c[i * n + j] = 0;
            for (int q = 0; q < k; ++q)
                c[i * n + j] += a[i * k + q] * b[q * n + j];
        }
}

int main() {
    const int m = 16;
    const int n = 32;
    const int k = 256;

    // 根据 DataType 的大小分配内存
    DataType A[m * k];
    DataType B[k * n];
    DataType C[m * n];

    // 初始化矩阵 A 和 B
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < k; ++j)
            A[i * k + j] = 1;

    for (int i = 0; i < k; ++i)
        for (int j = 0; j < n; ++j)
            B[i * n + j] = 1;

    // 执行 GEMM 操作
    gemm(A, B, C, m, n, k);


    return 0;
}
