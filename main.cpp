#include <iostream>
#include <chrono>
#include <cstring>
#include <stdlib.h>
#include "Head.hpp"

// 行优先储存
// 行优先和列优先主要是看储存二维数组元素的时候是有限储存那一维度的元素
#define a(i, j) a[(i)*lda + (j)]
#define b(i, j) b[(i)*ldb + (j)]
#define c(i, j) C[(j)*ldc + (i)]
#define abs(x) ((x) < 0.0 ? -(x) : (x)) 

int main(int argc, char**argv)
{
    // 计算的矩阵大小为 matrix_size * matrix_size
    // 测试时，可直接在此处设置矩阵大小
    int matrix_size = 1024;
    // 上面进行矩阵大小的设置

    int M = matrix_size;
    int K = matrix_size;
    int N = matrix_size;

    int size_A = sizeof(float) * M * K;
    int size_B = sizeof(float) * K * N;
    int size_C = sizeof(float) * M * N;

    float *A = (float*)malloc(size_A);
    float *B = (float*)malloc(size_B);
    float *C_Base = (float*)malloc(size_C);
    float *C_User = (float*)malloc(size_C);

    // 设置生成A,B随机数矩阵
    random_matrix(M, K, A, M);
    random_matrix(K, N, B, K);

    // 初始化【填充】结果数组
    memset(C_Base, 0, size_C);
    memset(C_User, 0, size_C);
    
    printf("=========================================\n");

    auto t1 = std::chrono::steady_clock::now();
    //开始计算和计时
    //////可修改函数User_matMult在solve.cpp中///////
// 这里要去看下两边的K,M,N意思是否是一致的
// 对，应该是一致的

    optimized_code(A, B, C_User, M, K, N, K, N, N);
//    User_matMult(A, B, C_User, M, K, N, K, N, N);
    ///////////////////////////////////////////
    auto t2 = std::chrono::steady_clock::now();
    int User_time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count(); 
    printf("| Your matMult Cost \x1b[33m%d ms\x1b[0m\n", User_time);

    // 也许使用上面这个计时方法就可以不用cuda计时的api？
    // 不管，先把这个想法实现了，后面还是要去修改尝试cuda的计时api

    auto t3 = std::chrono::steady_clock::now();
    Base_matMult(A, B, C_Base, M, K, N, K, N, N);
    auto t4 = std::chrono::steady_clock::now();
    int base_time = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count();
    printf("| Base matMult Cost \x1b[33m%d ms\x1b[0m\n", base_time);

    compare_matrices(M, N, C_Base, N, C_User, N);
    double speedup = double(base_time) / double(User_time) ;
    printf("| \x1b[34mTest passed\x1b[0m\n");
    printf("| \x1b[34mSpeedup factor: %.2f\x1b[0m\n", speedup);
    printf("=========================================\n");
   
    free(A);
    free(B);
    free(C_Base);
    free(C_User);

    return 0;
}


// 需要学习memset函数
// 学习上面的计时方式
// 项目实现后记得复现cuda的计时函数