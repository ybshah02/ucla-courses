#ifndef GEMM_H_
#define GEMM_H_

#include <vector>

const int kN = 4096;
const int kI = kN;
const int kJ = kN;
const int kK = kN;

void GemmBaseline(const float a[kI][kK], const float b[kK][kJ],
                  float c[kI][kJ]);
void GemmSequential(const float a[kI][kK], const float b[kK][kJ],
                    float c[kI][kJ]);
void GemmParallel(const float a[kI][kK], const float b[kK][kJ],
                  float c[kI][kJ]);
void GemmParallelBlocked(const float a[kI][kK], const float b[kK][kJ],
                         float c[kI][kJ]);
void Benchmark(
    void (*gemm)(const float[kI][kK], const float[kK][kJ], float[kI][kJ]),
    const float a[kI][kK], const float b[kK][kJ], float c[kI][kJ]);

void Init(float a[kI][kK], float[kK][kJ]);
int Diff(const float c1[kI][kJ], const float c2[kI][kJ]);
#endif
