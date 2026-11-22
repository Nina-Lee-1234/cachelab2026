/*
请注意，你的代码不能出现任何 int/short/char/float/double/auto 等局部变量/函数传参，我们仅允许使用 reg 定义的寄存器变量。
其中 reg 等价于一个 int。

你不能自己申请额外的内存，即不能使用 new/malloc，作为补偿我们传入了一段 buffer，大小为 BUFFER_SIZE = 64，你可以视情况使用。

我们的数组按照 A, B, C, buffer 的顺序在内存上连续紧密排列，且 &A = 0x30000000（这是模拟的设定，不是 A 的真实地址）

如果你需要以更自由的方式访问内存，你可以以相对 A 的方式访问，比如 A[100]，用 *(0x30000000) 是无法访问到的。

如果你有定义常量的需求（更严谨的说法是，你想定义的是汇编层面的立即数，不应该占用寄存器），请参考下面这种方式使用宏定义来完成。
*/

#include "cachelab.h"

#define m case0_m
#define n case0_n
#define p case0_p

void gemm_case0(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer)
{
  for (reg i = 0; i < m; ++i)
  {
    for (reg j = 0; j < p; ++j)
    {
      reg tmpc = 0;
      for (reg k = 0; k < n; ++k)
      {
        reg tmpa = A[i * n + k];
        reg tmpb = B[k * p + j];
        tmpc += tmpa * tmpb;
      }
      C[i * p + j] = tmpc;
    }
  }
}

#undef m
#undef n
#undef p

#define m case1_m
#define n case1_n
#define p case1_p

void gemm_case1(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer)
{
  for (reg i = 0; i < m; i += 8)
  {
    for (reg j = 0; j < p; j += 8)
    {
      for (reg ii = i; ii < i + 8; ii += 4)
      {
        for (reg jj = j; jj < j + 8; jj += 4)
        {
          reg c00 = 0, c01 = 0, c02 = 0, c03 = 0;
          reg c10 = 0, c11 = 0, c12 = 0, c13 = 0;
          reg c20 = 0, c21 = 0, c22 = 0, c23 = 0;
          reg c30 = 0, c31 = 0, c32 = 0, c33 = 0;

          for (reg k = 0; k < n; ++k)
          {
            reg a0 = A[ii * n + k];
            reg a1 = A[(ii + 1) * n + k];
            reg a2 = A[(ii + 2) * n + k];
            reg a3 = A[(ii + 3) * n + k];

            reg b0 = B[k * p + jj];
            reg b1 = B[k * p + jj + 1];
            reg b2 = B[k * p + jj + 2];
            reg b3 = B[k * p + jj + 3];

            c00 += a0 * b0; c01 += a0 * b1; c02 += a0 * b2; c03 += a0 * b3;
            c10 += a1 * b0; c11 += a1 * b1; c12 += a1 * b2; c13 += a1 * b3;
            c20 += a2 * b0; c21 += a2 * b1; c22 += a2 * b2; c23 += a2 * b3;
            c30 += a3 * b0; c31 += a3 * b1; c32 += a3 * b2; c33 += a3 * b3;
          }

          C[ii * p + jj] = c00; C[ii * p + jj + 1] = c01; C[ii * p + jj + 2] = c02; C[ii * p + jj + 3] = c03;
          C[(ii + 1) * p + jj] = c10; C[(ii + 1) * p + jj + 1] = c11; C[(ii + 1) * p + jj + 2] = c12; C[(ii + 1) * p + jj + 3] = c13;
          C[(ii + 2) * p + jj] = c20; C[(ii + 2) * p + jj + 1] = c21; C[(ii + 2) * p + jj + 2] = c22; C[(ii + 2) * p + jj + 3] = c23;
          C[(ii + 3) * p + jj] = c30; C[(ii + 3) * p + jj + 1] = c31; C[(ii + 3) * p + jj + 2] = c32; C[(ii + 3) * p + jj + 3] = c33;
        }
      }
    }
  }
}

#undef m
#undef n
#undef p

#define m case2_m
#define n case2_n
#define p case2_p

void gemm_case2(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer)
{
  for (reg i = 0; i < m; i += 8)
  {
    for (reg j = 0; j < p; j += 8)
    {
      for (reg ii = i; ii < i + 8; ii += 4)
      {
        for (reg jj = j; jj < j + 8; jj += 4)
        {
          reg c00 = 0, c01 = 0, c02 = 0, c03 = 0;
          reg c10 = 0, c11 = 0, c12 = 0, c13 = 0;
          reg c20 = 0, c21 = 0, c22 = 0, c23 = 0;
          reg c30 = 0, c31 = 0, c32 = 0, c33 = 0;

          for (reg k = 0; k < n; ++k)
          {
            reg a0 = A[ii * n + k];
            reg a1 = A[(ii + 1) * n + k];
            reg a2 = A[(ii + 2) * n + k];
            reg a3 = A[(ii + 3) * n + k];

            reg b0 = B[k * p + jj];
            reg b1 = B[k * p + jj + 1];
            reg b2 = B[k * p + jj + 2];
            reg b3 = B[k * p + jj + 3];

            c00 += a0 * b0; c01 += a0 * b1; c02 += a0 * b2; c03 += a0 * b3;
            c10 += a1 * b0; c11 += a1 * b1; c12 += a1 * b2; c13 += a1 * b3;
            c20 += a2 * b0; c21 += a2 * b1; c22 += a2 * b2; c23 += a2 * b3;
            c30 += a3 * b0; c31 += a3 * b1; c32 += a3 * b2; c33 += a3 * b3;
          }

          C[ii * p + jj] = c00; C[ii * p + jj + 1] = c01; C[ii * p + jj + 2] = c02; C[ii * p + jj + 3] = c03;
          C[(ii + 1) * p + jj] = c10; C[(ii + 1) * p + jj + 1] = c11; C[(ii + 1) * p + jj + 2] = c12; C[(ii + 1) * p + jj + 3] = c13;
          C[(ii + 2) * p + jj] = c20; C[(ii + 2) * p + jj + 1] = c21; C[(ii + 2) * p + jj + 2] = c22; C[(ii + 2) * p + jj + 3] = c23;
          C[(ii + 3) * p + jj] = c30; C[(ii + 3) * p + jj + 1] = c31; C[(ii + 3) * p + jj + 2] = c32; C[(ii + 3) * p + jj + 3] = c33;
        }
      }
    }
  }
}

#undef m
#undef n
#undef p

#define m case3_m
#define n case3_n
#define p case3_p

#define MR 4
#define K 8

void gemm_case3(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer)
{
  for (reg ii = 0; ii < m; ii += MR)
  {
    for (reg kk = 0; kk < n; kk += K)
    {
      for (reg r = 0; r < MR; ++r)
      {
        for (reg c = 0; c < K; ++c)
        {
          reg val = 0;
          if (ii + r < m && kk + c < n)
            val = A[(ii + r) * n + kk + c];
          buffer[r * K + c] = val;
        }
      }
      for (reg jj = 0; jj < p; jj += MR)
      {
        reg c00 = 0, c01 = 0, c02 = 0, c03 = 0;
        reg c10 = 0, c11 = 0, c12 = 0, c13 = 0;
        reg c20 = 0, c21 = 0, c22 = 0, c23 = 0;
        reg c30 = 0, c31 = 0, c32 = 0, c33 = 0;
        if (kk != 0) {
          if (ii < m && jj < p) c00 = C[ii * p + jj];
          if (ii < m && jj + 1 < p) c01 = C[ii * p + jj + 1];
          if (ii < m && jj + 2 < p) c02 = C[ii * p + jj + 2];
          if (ii < m && jj + 3 < p) c03 = C[ii * p + jj + 3];
          if (ii + 1 < m && jj < p) c10 = C[(ii + 1) * p + jj];
          if (ii + 1 < m && jj + 1 < p) c11 = C[(ii + 1) * p + jj + 1];
          if (ii + 1 < m && jj + 2 < p) c12 = C[(ii + 1) * p + jj + 2];
          if (ii + 1 < m && jj + 3 < p) c13 = C[(ii + 1) * p + jj + 3];
          if (ii + 2 < m && jj < p) c20 = C[(ii + 2) * p + jj];
          if (ii + 2 < m && jj + 1 < p) c21 = C[(ii + 2) * p + jj + 1];
          if (ii + 2 < m && jj + 2 < p) c22 = C[(ii + 2) * p + jj + 2];
          if (ii + 2 < m && jj + 3 < p) c23 = C[(ii + 2) * p + jj + 3];
          if (ii + 3 < m && jj < p) c30 = C[(ii + 3) * p + jj];
          if (ii + 3 < m && jj + 1 < p) c31 = C[(ii + 3) * p + jj + 1];
          if (ii + 3 < m && jj + 2 < p) c32 = C[(ii + 3) * p + jj + 2];
          if (ii + 3 < m && jj + 3 < p) c33 = C[(ii + 3) * p + jj + 3];
        }
        for (reg k = 0; k < K; ++k)
        {
          if (kk + k >= n) break;
          reg a0 = buffer[0 * K + k];
          reg a1 = buffer[1 * K + k];
          reg a2 = buffer[2 * K + k];
          reg a3 = buffer[3 * K + k];
          reg b0 = 0, b1 = 0, b2 = 0, b3 = 0;
          if (jj < p) b0 = B[(kk + k) * p + jj];
          if (jj + 1 < p) b1 = B[(kk + k) * p + jj + 1];
          if (jj + 2 < p) b2 = B[(kk + k) * p + jj + 2];
          if (jj + 3 < p) b3 = B[(kk + k) * p + jj + 3];
          c00 += a0 * b0; c01 += a0 * b1; c02 += a0 * b2; c03 += a0 * b3;
          c10 += a1 * b0; c11 += a1 * b1; c12 += a1 * b2; c13 += a1 * b3;
          c20 += a2 * b0; c21 += a2 * b1; c22 += a2 * b2; c23 += a2 * b3;
          c30 += a3 * b0; c31 += a3 * b1; c32 += a3 * b2; c33 += a3 * b3;
        }
        if (ii < m && jj < p) C[ii * p + jj] = c00;
        if (ii < m && jj + 1 < p) C[ii * p + jj + 1] = c01;
        if (ii < m && jj + 2 < p) C[ii * p + jj + 2] = c02;
        if (ii < m && jj + 3 < p) C[ii * p + jj + 3] = c03;
        if (ii + 1 < m && jj < p) C[(ii + 1) * p + jj] = c10;
        if (ii + 1 < m && jj + 1 < p) C[(ii + 1) * p + jj + 1] = c11;
        if (ii + 1 < m && jj + 2 < p) C[(ii + 1) * p + jj + 2] = c12;
        if (ii + 1 < m && jj + 3 < p) C[(ii + 1) * p + jj + 3] = c13;
        if (ii + 2 < m && jj < p) C[(ii + 2) * p + jj] = c20;
        if (ii + 2 < m && jj + 1 < p) C[(ii + 2) * p + jj + 1] = c21;
        if (ii + 2 < m && jj + 2 < p) C[(ii + 2) * p + jj + 2] = c22;
        if (ii + 2 < m && jj + 3 < p) C[(ii + 2) * p + jj + 3] = c23;
        if (ii + 3 < m && jj < p) C[(ii + 3) * p + jj] = c30;
        if (ii + 3 < m && jj + 1 < p) C[(ii + 3) * p + jj + 1] = c31;
        if (ii + 3 < m && jj + 2 < p) C[(ii + 3) * p + jj + 2] = c32;
        if (ii + 3 < m && jj + 3 < p) C[(ii + 3) * p + jj + 3] = c33;
      }
    }
  }
}

#undef m
#undef n
#undef p