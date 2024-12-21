#include <iostream>
#include "../include/Head.hpp"
// 循环展开的矩阵乘法函数（按4次展开，处理边界情况）
// void optimized_code(float* A, float* B, float* C, int M, int K, int N, int lda, int ldb, int ldc) {
//     for (int i = 0; i < M; ++i) {
//         int j_end = N;
//         if (N % 4!= 0) {
//             j_end = N - (N % 4);
//         }
//         for (int j = 0; j < j_end; j += 4) {
//             // 用于累加的临时变量，对应每次展开的4列结果
//             float sum0 = 0.0f, sum1 = 0.0f, sum2 = 0.0f, sum3 = 0.0f;
//             for (int k = 0; k < K; ++k) {
//                 // 根据行优先存储方式计算对应元素在一维数组中的索引并进行乘法累加操作
//                 sum0 += A[i * lda + k] * B[k * ldb + j];
//                 sum1 += A[i * lda + k] * B[k * ldb + j + 1];
//                 sum2 += A[i * lda + k] * B[k * ldb + j + 2];
//                 sum3 += A[i * lda + k] * B[k * ldb + j + 3];
//             }
//             // 将累加结果赋值给结果矩阵对应的位置（同样根据行优先存储计算索引）
//             C[i * ldc + j] += sum0;
//             C[i * ldc + j + 1] += sum1;
//             C[i * ldc + j + 2] += sum2;
//             C[i * ldc + j + 3] += sum3;
//         }
//         // 处理剩余不足4列的情况（边界情况处理）
//         if (N % 4!= 0) {
//             int remaining_cols = N % 4;
//             for (int j_remain = j_end; j_remain < N; ++j_remain) {
//                 float sum_remain = 0.0f;
//                 for (int k = 0; k < K; ++k) {
//                     sum_remain += A[i * lda + k] * B[k * ldb + j_remain];
//                 }
//                 C[i * ldc + j_remain] += sum_remain;
//             }
//         }
//     }
// }
void optimized_code(float* A, float* B, float* C, int M, int K, int N, int lda, int ldb, int ldc){
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < K; k++)
            {
                C[(j)*ldc + (i)] += A[(i)*lda + (j)] * B[(i)*ldb + (j)];
            }
        }
    }
}