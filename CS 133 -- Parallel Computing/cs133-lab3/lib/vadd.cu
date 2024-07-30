#include <chrono>
#include <iostream>
#include <random>

#include "utils.cuh"

using std::clog;
using std::endl;

// VADD kernel
__global__ void vadd_gpu(const float* g_a, const float* g_b, float* g_c, uint64_t n) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n) {
    g_c[i] = g_a[i] + g_b[i];
  }
}

int main(int argc, const char* argv[]) {
  const uint64_t n = 1 << 20;
  static float a[n];
  static float b[n];
  static float c[n];
  static float c_base[n];

  // generate data
  std::default_random_engine generator(
      std::chrono::steady_clock::now().time_since_epoch().count());
  std::uniform_real_distribution<float> distribution(0.f, 1.f);
  for (uint64_t i = 0; i < n; ++i) {
    a[i] = distribution(generator);
    b[i] = distribution(generator);
    c_base[i] = a[i] + b[i];
  }

  // create device memory
  float* g_a, *g_b, *g_c;
  clog << "Create device memory\n";
  cudaMalloc((float**)&g_a, n*sizeof(float));
  cudaMalloc((float**)&g_b, n*sizeof(float));
  cudaMalloc((float**)&g_c, n*sizeof(float));

  // transfer to global memory
  clog << "Transfer to global memory\n";
  cudaMemcpy(g_a, a, n*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(g_b, b, n*sizeof(float), cudaMemcpyHostToDevice);

  // set params
  auto block = get_param("BLOCK");
  auto grid = get_param("GRID");
  std::clog << "Using thread block dims: " << block.x << ' ' << block.y << ' ' << block.z << '\n';
  std::clog << "Using grid dims: " << grid.x << ' ' << grid.y << ' ' << grid.z << '\n';

  // set device
  cudaSetDevice(0);
  
  // execute kernel
  clog << "Invoke VADD computation kernel\n";
  vadd_gpu<<<grid, block>>>(g_a, g_b, g_c, n);
  CUDA_CHECK(cudaDeviceSynchronize()); // wait until kernel is completely finished

  // get back the data
  cudaMemcpy(c, g_c, n*sizeof(float), cudaMemcpyDeviceToHost);

  // check correctness
  for (uint64_t i = 0; i < n; ++i) {
    if (c[i] != c_base[i]) {
      clog << "FAIL" << i << endl;
      return 1;
    }
  }
  clog << "PASS" << endl;
  return 0;
}