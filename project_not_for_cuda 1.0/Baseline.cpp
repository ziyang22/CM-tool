#include <iostream>
#include <stdlib.h>

#define A(i, j) A[(i)*lda + (j)]
#define B(i, j) B[(i)*ldb + (j)]
#define C(i, j) C[(j)*ldc + (i)]
#define abs(x) ((x) < 0.0 ? -(x) : (x)) 

void Base_matMult(float* A, float* B, float* C, int M, int K, int N, int lda, int ldb, int ldc){
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < K; k++)
            {
                C(i, j) += A(i, k) * B(k, j);
            }
        }
    }
}