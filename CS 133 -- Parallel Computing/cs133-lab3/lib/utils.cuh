#ifndef UTILS_CUH
#define UTILS_CUH

#include <stdexcept>
#include <string>

#define CUDA_CHECK(err) cuda_check((err), __FILE__, __LINE__);
inline void cuda_check(cudaError_t err, const char* file, int line)
{
  if (err != cudaSuccess) {
    throw std::runtime_error(
        std::string(file) + ":" + std::to_string(line) + ": " + std::string(cudaGetErrorString(err)));
  }
}

dim3 get_param(std::string setting);

void LoadData(
  const std::string& data_dir,
  float* input,
  float* weight, float* bias
);

int Verify(
  const std::string& data_dir,
  const float* output,
  int output_size
);

#endif