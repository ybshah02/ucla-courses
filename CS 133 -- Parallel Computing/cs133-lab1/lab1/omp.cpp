// Header inclusions, if any...

#include "lib/gemm.h"
#include <cstring>

void GemmParallel(const float a[kI][kK], const float b[kK][kJ], float c[kI][kJ]) {
    // zero out matrix c
    for (int k = 0; k < kI; ++k) {
        std::memset(c[k], 0, sizeof(float) * kJ);
    }
    
    #pragma omp parallel for
    for (int i = 0; i < kI; i++) {
        for (int k = 0; k < kK; k++) {
            for (int j = 0; j < kJ; j++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}
