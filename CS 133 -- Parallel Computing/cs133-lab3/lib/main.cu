#include <chrono>
#include <iostream>
#include <string>

#include "cnn.cuh"
#include "utils.cuh"
#include "cnn_seq.cuh"
#include "../cnn_gpu.cuh"

using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::steady_clock;
using std::clog;
using std::string;

void cnn_gpu_wrapper(float* g_input,float* g_weight,float* g_bias,float* g_output) {
  // get params
  auto block = get_param("BLOCK");
  auto grid = get_param("GRID");
  std::clog << "Using thread block dims: " << block.x << ' ' << block.y << ' ' << block.z << '\n';
  std::clog << "Using grid dims: " << grid.x << ' ' << grid.y << ' ' << grid.z << '\n';
  
  // set device
  cudaSetDevice(0);
  
  // execute kernel
  cnn_gpu<<<grid, block>>>(g_input, g_weight, g_bias, g_output);
  CUDA_CHECK(cudaDeviceSynchronize()); // wait until kernel is completely finished
}

int main(int argc, char** argv) {

  // sizes are known ahead of time for this particular example
  auto input_size = kNum * kInImSize * kInImSize * sizeof(float);
  auto weight_size = kNum * kNum * kKernel * kKernel * sizeof(float);
  auto bias_size = kNum * sizeof(float);
  auto output_size = kNum * kOutImSize * kOutImSize * sizeof(float);

  // allocate memory on heap to avoid stack overflow
  float* input = static_cast<float*>(malloc(input_size));
  float* weight = static_cast<float*>(malloc(weight_size));
  float* bias = static_cast<float*>(malloc(bias_size));
  float* output = static_cast<float*>(malloc(output_size));

  if (argc > 2) {
    clog << "Usage: " << argv[0] << " [data dir]\n";
    return EXIT_FAILURE;
  }

  // load data
  const string data_dir = argc == 2 ? string(argv[1]) + "/" : "lib/data/";
  LoadData(data_dir, input, weight, bias);

  // create device memory
  float* g_input, *g_weight, *g_bias, *g_output;
  if (!getenv("SEQUENTIAL")) {
    clog << "Create device memory\n";
    cudaMalloc((float**)&g_input, input_size);
    cudaMalloc((float**)&g_weight, weight_size);
    cudaMalloc((float**)&g_bias, bias_size);
    cudaMalloc((float**)&g_output, output_size);
  }

  // transfer to global memory
  if (!getenv("SEQUENTIAL")) {
    clog << "Transfer to global memory\n";
    cudaMemcpy(g_input, input, input_size, cudaMemcpyHostToDevice);
    cudaMemcpy(g_weight, weight, weight_size, cudaMemcpyHostToDevice);
    cudaMemcpy(g_bias, bias, bias_size, cudaMemcpyHostToDevice);
  }

  // invoke CNN kernel
  clog << "Invoke CNN computation kernel\n";
  auto begin = steady_clock::now();
  if (getenv("SEQUENTIAL")) {
    cnn_seq(input, weight, bias, output);
  } else {
    cnn_gpu_wrapper(g_input, g_weight, g_bias, g_output);
  }
  auto end = steady_clock::now();
  uint64_t run_time_us = duration_cast<microseconds>(end - begin).count();
  float gflops = float(kNum) * kNum * kImSize * kImSize * kKernel * kKernel * 2
    / (run_time_us * 1e3);
  clog << "Time: " << run_time_us * 1e-6 << " s\n";
  clog << "Perf: " << gflops << " GFlops\n";

  // get the data back
  if (!getenv("SEQUENTIAL")) {
    cudaMemcpy(output, g_output, output_size, cudaMemcpyDeviceToHost);
  }

  // verify correctness
  int error = Verify(data_dir, output, output_size);
  if (error != 0) {
    clog << "Found " << error << " error" << (error > 1 ? "s\n" : "\n");
    clog << "FAIL\n";
    return EXIT_FAILURE;
  } else {
    clog << "PASS\n";
    return EXIT_SUCCESS;
  }
}
