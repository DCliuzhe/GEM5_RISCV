#include <stdio.h>
#include <riscv_vector.h>

void gemm(double *a, double *b, double *c, int n, int m, int p) {
       size_t vlmax = __riscv_vsetvlmax_e64m1();
       printf("%u", vlmax);
       for (int i = 0; i < n; ++i)
         for (int j = 0; j < m; ++j) {
           double *ptr_a = &a[i * p];
           double *ptr_b = &b[j];
           int k = p;
           // Set accumulator to  zero.
           vfloat64m1_t vec_s = __riscv_vfmv_v_f_f64m1(0.0, vlmax);
           vfloat64m1_t vec_zero = __riscv_vfmv_v_f_f64m1(0.0, vlmax);
           for (size_t vl; k > 0; k -= vl) {
             vl = __riscv_vsetvl_e64m1(k);
             // Load row a[i][k..k+vl)
             vfloat64m1_t vec_a = __riscv_vle64_v_f64m1(ptr_a, vl);
             // Load column b[k..k+vl)[j]
             vfloat64m1_t vec_b =
               __riscv_vlse64_v_f64m1(ptr_b, sizeof(double) * m, vl);
             // Accumulate dot product of row and column. If vl < vlmax we need to
             // preserve the existing values of vec_s, hence the tu policy.
             vec_s = __riscv_vfmacc_vv_f64m1_tu(vec_s, vec_a, vec_b, vl);
}
           // Final accumulation.
           vfloat64m1_t vec_sum =
             __riscv_vfredusum_vs_f64m1_f64m1(vec_s, vec_zero, vlmax);
           double sum = __riscv_vfmv_f_s_f64m1_f64(vec_sum);
           c[i * m + j] = sum;
} }

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