#ifndef CNN_GPU_CUH
#define CNN_GPU_CUH

__global__ void cnn_gpu(
  float* input,
  float* weight,
  float* bias,
  float* output
);

#endif