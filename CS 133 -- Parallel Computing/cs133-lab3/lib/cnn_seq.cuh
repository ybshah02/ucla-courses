#ifndef CNN_SEQ_CUH
#define CNN_SEQ_CUH

#include "cnn.cuh"

#define C(i,h,w) C[(i)*kImSize*kImSize + (h)*kImSize + (w)]

void cnn_seq(
  const float* input,
  const float* weight,
  const float* bias,
  float* output
);

#endif