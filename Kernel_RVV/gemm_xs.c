#include <riscv_vector.h>
#include <stdio.h>
#include <stdlib.h>

void matmul(float *a, float *b, float *c, int M, int N, int K) {
  for (int i = 0; i < M; ++i) {
    for (int j = 0; j < N; ++j) {
        int k = 0;
        c[i * N + j] = 0;
        for(size_t vl; k < K; ){
            vl = __riscv_vsetvl_e32m1(K - k);
            vfloat32m1_t vec_a = __riscv_vle32_v_f32m1(&a[i * K + k], vl);
            vfloat32m1_t vec_b = __riscv_vle32_v_f32m1(&b[j * K + k], vl);
            vfloat32m1_t vec_s = __riscv_vfmul_vv_f32m1(vec_a, vec_b, vl);
            vfloat32m1_t vsum = __riscv_vfredusum_vs_f32m1_f32m1(vec_s, __riscv_vfmv_s_f_f32m1(0.0f, vl), vl);
            float sum = __riscv_vfmv_f_s_f32m1_f32(vsum);
            printf("sum : %f \n", sum);
            c[i * N + j] += sum;
            k += vl;
        }
    }
  }
}

int main() {
    int M = 16;
    int N = 16;
    int K = 64;

    float *a = (float*)malloc(M * K * sizeof(float));
    float *b = (float*)malloc(N * K * sizeof(float));
    float *c = (float*)malloc(M * N * sizeof(float));

    for(int i = 0; i < M * K; i ++)
        a[i] = 1.0f;
    
    for(int i = 0; i < N * K; i ++)
        b[i] = 2.0f;
    
    
    matmul(a, b, c, M, N, K);

    for(int i = 0; i < M * N; i ++)
        printf("%f ", c[i]);
    
    return 0;




}