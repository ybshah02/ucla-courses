// Header inclusions, if any...
#include "lib/cnn.cuh"
#include "cnn_gpu.cuh"

__global__ void cnn_gpu(
    float *input,
    float *weight,
    float *bias,
    float *output)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int h = blockIdx.y * blockDim.y + threadIdx.y;
    int w = blockIdx.z * blockDim.z + threadIdx.z;

    h *= 2;
    w *= 2;

    if (i < kNum && h < kImSize && w < kImSize) {
        float conv[4] = {bias[i], bias[i], bias[i], bias[i]};

        for (int j = 0; j < kNum; ++j) {
            for (int p = 0; p < kKernel; ++p) {
                for (int q = 0; q < kKernel; ++q) {
                    if (h + p < kInImSize && w + q < kInImSize) {
                        conv[0] += weight(i, j, p, q) * input(j, h+p, w+q);
                        conv[1] += weight(i, j, p, q) * input(j, h+1+p, w+q);
                        conv[2] += weight(i, j, p, q) * input(j, h+p, w+1+q);
                        conv[3] += weight(i, j, p, q) * input(j, h+1+p, w+1+q);
                    }
                }
            }
        }

        float final_conv = max(max(conv[0], conv[1]), max(conv[2], conv[3]));
        if (h / 2 < kOutImSize && w / 2 < kOutImSize) {
            output(i, h / 2, w / 2) = max(0.f, final_conv);
        }
    }
}
