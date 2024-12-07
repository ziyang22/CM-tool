#ifndef HEAD_HPP
#define HEAD_HPP


//你的代码写在这里
#include <iostream>
#include <stdlib.h>


// 行优先储存
// 行优先和列优先主要是看储存二维数组元素的时候是有限储存那一维度的元素
#define a(i, j) a[(i)*lda + (j)]
#define b(i, j) b[(i)*ldb + (j)]
#define c(i, j) C[(j)*ldc + (i)]
#define abs(x) ((x) < 0.0 ? -(x) : (x)) 

void Base_matMult(float* A, float* B, float* C, int M, int K, int N, int lda, int ldb, int ldc);
void random_matrix(int m, int n, float *C, int ldc);
void compare_matrices(int m, int n, float *a, int lda, float *b, int ldb);
void optimized_code(float* A, float* B, float* C, int M, int K, int N, int lda, int ldb, int ldc);

#endif