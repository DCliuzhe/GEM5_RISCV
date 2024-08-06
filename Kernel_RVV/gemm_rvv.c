#include <stdio.h>
#include <riscv_vector.h>

#define N 128

// 矩阵乘法函数
void matmul_rvv(const float A[N][N], const float B[N][N], float C[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0.0f;  // 初始化 C 矩阵的元素为 0
            for (int k = 0; k < N; ) {
                // 设置和调整矢量长度 VL
                size_t vl = __riscv_vsetvl_e32m1(N - k);

                // 加载 A 和 B 的元素到矢量寄存器
                vfloat32m1_t va = __riscv_vle32_v_f32m1(&A[i][k], vl);
                vfloat32m1_t vb = __riscv_vle32_v_f32m1(&B[k][j], vl);

                // 矢量乘法
                vfloat32m1_t vproduct = __riscv_vfmul_vv_f32m1(va, vb, vl);

                // 矢量缩减累加，累加所有矢量元素的值
                vfloat32m1_t vsum = __riscv_vfredosum_vs_f32m1_f32m1(vproduct, __riscv_vfmv_s_f_f32m1(0.0f, vl), vl);

                // 提取标量值
                float sum = __riscv_vfmv_f_s_f32m1_f32(vsum);

                // 将累加结果加到 C 矩阵中
                C[i][j] += sum;

                // 增量 k
                k += vl;
            }
        }
    }
}


int main() {
    // 初始化矩阵 A 和 B 为全 1 的 16x16 浮点矩阵
    float A[N][N];
    float B[N][N];
    float C[N][N];

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = 1;
            B[i][j] = 1;
        }
    }

    matmul_rvv(A, B, C);

    printf("Matrix C:\n");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            printf("%f ", C[i][j]);
        }
        printf("\n");
    }

    return 0;
}
