#include <stdio.h>
#include <riscv_vector.h>

void gemm_rvv_outer_product(float *A, float *B, float *C, int M, int N, int K) {
    // 假设向量长度为 N，数据类型为 32 位浮点数
    size_t vlmax = __riscv_vsetvlmax_e32m1();

    for (int i = 0; i < M; ++i) {
        // 指向矩阵 C 的第 i 行
        float *ptr_c = &C[i * N];

        // 初始化 C 的第 i 行为 0
        size_t n = N;
        size_t vl;
        size_t offset = 0;

        // 使用循环处理可能无法整除的情况
        while (n > 0) {
            vl = __riscv_vsetvl_e32m1(n); // 设置当前的向量长度

            vfloat32m1_t vc = __riscv_vfmv_v_f_f32m1(0.0f, vl); // 初始化累加器为 0

            for (int k = 0; k < K; ++k) {
                // 加载 A 的元素 A_{i,k}
                float a_ik = A[i * K + k];

                // 加载 B 的第 k 行，从 offset 开始，长度为 vl
                float *ptr_bk = &B[k * N + offset];
                vfloat32m1_t vb = __riscv_vle32_v_f32m1(ptr_bk, vl);

                // 计算 temp = A_{i,k} * B_{k,:}
                vfloat32m1_t vtemp = __riscv_vfmul_vf_f32m1(vb, a_ik, vl);

                // 累加到 vc
                vc = __riscv_vfadd_vv_f32m1(vc, vtemp, vl);
            }

            // 存储结果到 C 的第 i 行，从 offset 开始
            __riscv_vse32_v_f32m1(&ptr_c[offset], vc, vl);

            n -= vl;
            offset += vl;
        }
    }
}

int main() {
    // 示例矩阵尺寸
    int M = 48;
    int K = 64;
    int N = 32;

    // 分配和初始化矩阵 A、B、C
    float A[M * K];
    float B[K * N];
    float C[M * N];

    // 将矩阵 A 初始化为 1
    for (int i = 0; i < M * K; ++i) {
        A[i] = 1.0f;
    }

    // 将矩阵 B 初始化为 1
    for (int i = 0; i < K * N; ++i) {
        B[i] = 1.0f;
    }

    // 将矩阵 C 初始化为 0
    for (int i = 0; i < M * N; ++i) {
        C[i] = 0.0f;
    }

    // 调用 RVV 实现的 GEMM 函数
    gemm_rvv_outer_product(A, B, C, M, N, K);
    
    return 0;
}
