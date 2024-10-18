#include <stdio.h>
#include <riscv_vector.h>

void gemm(int8_t *a, int8_t *b, int8_t *c, int n, int m, int p) {
    size_t vlmax = __riscv_vsetvlmax_e8m4();  // 获取支持的最大向量长度
    printf("VLMax: %lu\n", vlmax);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            int8_t *ptr_a = &a[i * p];
            int8_t *ptr_b = &b[j];
            int k = p;
            // Set accumulator to zero.
            vint8m1_t vec_s = __riscv_vmv_v_x_i8m4(0, vlmax);  // 使用 int8_t 类型的累加器
            for (size_t vl; k > 0; k -= vl) {
                vl = __riscv_vsetvl_e8m4(k); // 设置当前向量长度
                // Load row a[i][k..k+vl)
                vint8m1_t vec_a = __riscv_vle8_v_i8m4(ptr_a, vl);
                // Load column b[k..k+vl)[j]
                vint8m1_t vec_b = __riscv_vlse8_v_i8m4(ptr_b, sizeof(int8_t) * m, vl);
                // Perform element-wise multiplication and accumulation
                vec_s = __riscv_vmacc_vv_i8m4(vec_s, vec_a, vec_b, vl);
                ptr_a += vl;
                ptr_b += vl * m;
            }
            // Final accumulation using vector reduction sum
            vint8m1_t vec_zero = __riscv_vmv_v_x_i8m1(0, vlmax);
            vint8m1_t vec_sum = __riscv_vredsum_vs_i8m4_i8m1(vec_s, vec_zero, vlmax);
            int8_t sum = __riscv_vmv_x_s_i8m1_i8(vec_sum);
            c[i * m + j] = sum;  // 将累加结果存储到矩阵 C 中
        }
    }
}

int main() {
    const int m = 16;
    const int n = 16;
    const int k = 8192;

    int8_t A[m * k];
    int8_t B[k * n];
    int8_t C[m * n];

    // 初始化矩阵 A
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < k; ++j)
            A[i * k + j] = 1;

    // 初始化矩阵 B
    for (int i = 0; i < k; ++i)
        for (int j = 0; j < n; ++j)
            B[i * n + j] = 1;

    // 执行 GEMM 操作
    gemm(A, B, C, m, n, k);


    return 0;
}
