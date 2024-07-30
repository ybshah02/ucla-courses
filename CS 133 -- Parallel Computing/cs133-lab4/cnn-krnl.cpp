#include "lib/cnn-krnl.h"

#pragma ACCEL kernel
void CnnKernel(
    const input_t input[kNum][kInImSize][kInImSize],
    const weight_t weight[kNum][kNum][kKernel][kKernel],
    const bias_t bias[kNum],
    output_t output[kNum][kOutImSize][kOutImSize]
  ) {

  // Allocate memory on heap to avoid stack overflow.
  static compute_t C[kNum][kImSize][kImSize];

  for (int i = 0; i < kNum; ++i) {
    for (int h = 0; h < kImSize; ++h) {
      for (int w = 0; w < kImSize; ++w) {
        C[i][h][w] = bias[i];
      }
    }
  }

  // Convolution
  #pragma ACCEL pipeline
  for (int j = 0; j < kNum; ++j) {
    for (int i = 0; i < kNum; ++i) {
      for (int h = 0; h < kImSize; ++h) {
        #pragma ACCEL parallel factor=8
        for (int w = 0; w < kImSize; ++w) {
          for (int p = 0; p < kKernel; ++p) {
            for (int q = 0; q < kKernel; ++q) {
              C[i][h][w] += weight[i][j][p][q] * input[j][h + p][w + q];
            }
          }
        }
      }
    }
  }

  // ReLU
  for (int i = 0; i < kNum; ++i) {
    for (int h = 0; h < kImSize; ++h) {
      for (int w = 0; w < kImSize; ++w) {
        C[i][h][w] = max(0, C[i][h][w]);
      }
    }
  }

  // Max pooling
  for (int i = 0; i < kNum; ++i) {
    for (int h = 0; h < kOutImSize; ++h) {
      for (int w = 0; w < kOutImSize; ++w) {
        output[i][h][w] = max(
            max(C[i][h * 2][w * 2    ], C[i][h * 2 + 1][w * 2    ]),
            max(C[i][h * 2][w * 2 + 1], C[i][h * 2 + 1][w * 2 + 1]));
      }
    }
  }
}