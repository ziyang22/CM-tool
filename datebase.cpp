#include <iostream>
#include <stdlib.h>

// 行优先储存
// 行优先和列优先主要是看储存二维数组元素的时候是有限储存那一维度的元素
#define a(i, j) a[(i)*lda + (j)]
#define b(i, j) b[(i)*ldb + (j)]
#define c(i, j) C[(j)*ldc + (i)]
#define abs(x) ((x) < 0.0 ? -(x) : (x)) 



// 生成随机数矩阵，矩阵元素大小为-1~1的浮点数
// 矩阵行数为j，列数为i
void random_matrix(int m, int n, float *C, int ldc) {
    int i, j;

    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            //C(i, j) = 2.0 * (float)drand48() - 1.0;
             c(i, j) = (rand()%255)/255.0;
        }
    }
}

// drand48()是一个用于生成伪随机数的函数。它返回一个范围在[0.0, 1.0)之间的双精度浮点数（double类型）。
// 伪随机数是通过一定的算法生成的，看似随机但实际上是有规律的数字序列，在给定相同的初始条件（种子）下，生成的随机数序列是相同的。
// 有待进一步探索