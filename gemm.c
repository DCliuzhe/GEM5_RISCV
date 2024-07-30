#include <stdint.h>
#include <stdio.h>

// 定义一个内联汇编函数来读取 cycle 寄存器
static inline uint64_t read_cycle(void) {
    uint64_t cycle;
    asm volatile ("rdcycle %0" : "=r" (cycle));
    return cycle;
}

// 内联汇编函数，读取 time 寄存器
static inline uint64_t read_time(void) {
    uint64_t time;
    asm volatile ("rdtime %0" : "=r" (time));
    return time;
}

// 内联汇编函数，读取 instret 寄存器
static inline uint64_t read_instret(void) {
    uint64_t instret;
    asm volatile ("rdinstret %0" : "=r" (instret));
    return instret;
}

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
    // 读取开始时的周期数
    uint64_t start_cycle, end_cycle;
    uint64_t start_time, end_time;
    uint64_t start_instret, end_instret;

    start_cycle = read_cycle();
    start_time = read_time();
    start_instret = read_instret();

    const int N = 16;  // 矩阵大小
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

    // 读取结束时的周期数
    end_cycle = read_cycle();
    end_time = read_time();
    end_instret = read_instret();

    // 计算并打印周期数差值
    printf("Time: %lu\n", end_time - start_time);
    printf("Cycles: %lu\n", end_cycle - start_cycle);
    printf("Instructions: %lu\n", end_instret - start_instret);

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

