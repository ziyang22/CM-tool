#include "../include/Head.hpp"
#include <iostream>
#include <stdlib.h>


// 行优先储存
// 行优先和列优先主要是看储存二维数组元素的时候是有限储存那一维度的元素
#define a(i, j) a[(i)*lda + (j)]
#define b(i, j) b[(i)*ldb + (j)]
#define c(i, j) C[(j)*ldc + (i)]
#define abs(x) ((x) < 0.0 ? -(x) : (x)) 

// 定义了一个名为 compare_matrices 的函数，用于比较两个矩阵 a 和 b 的元素差异
// m为行数，n为列数
// lda和ldb为矩阵a和b的步长【每行元素个数】（为什么不直接用列数n来代替？）

void Base_matMult(float* A, float* B, float* C, int M, int K, int N, int lda, int ldb, int ldc){
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

void compare_matrices(int m, int n, float *a, int lda, float *b, int ldb) {
    int i, j;
    float max_diff = 0.0, diff;

    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            diff = abs(a(i, j) - b(i, j));
            max_diff = (diff > max_diff ? diff : max_diff);
            if (max_diff > 1.5 || max_diff < -1.5) {
                printf("| \x1b[31mTest failed !!!\x1b[0m\n");
                printf("| \x1b[31mFirst error in: i=%d j=%d\x1b[0m\n", i, j);
                printf("| \x1b[31mBase value: %f\x1b[0m\n", a(i, j));
                printf("| \x1b[31mYour value: %f\x1b[0m\n", b(i, j));
                printf("=========================================\n");
                exit(-1);
            }
        }
    }

}
