#include "lib/gemm.h"
#include <iostream>
#include <cstring>

#define BLOCK_SIZE 1024

// copy source block to destination block
void copyBlock(float dest[BLOCK_SIZE][BLOCK_SIZE], const float source[kI][kJ], int i_offset, int j_offset) {
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        for (int j = 0; j < BLOCK_SIZE; ++j) {
            dest[i][j] = source[i+i_offset][j+j_offset];
        }
    }
}

// update block in destination with values from source
void updateBlock(float dest[kI][kJ], const float src[BLOCK_SIZE][BLOCK_SIZE], int i_offset, int j_offset) {
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        for (int j = 0; j < BLOCK_SIZE; ++j) {
            dest[i+i_offset][j+j_offset] = src[i][j];
        }
    }
}

void GemmParallelBlocked(const float a[kI][kK], const float b[kK][kJ], float c[kI][kJ]) {
    // zero out matrix c
    for (int k = 0; k < kI; ++k) {
        std::memset(c[k], 0, sizeof(float) * kJ);
    }

    static float block_a[BLOCK_SIZE][BLOCK_SIZE];
    static float block_b[BLOCK_SIZE][BLOCK_SIZE];
    static float block_c[BLOCK_SIZE][BLOCK_SIZE];

    for (int i = 0; i < kI; i += BLOCK_SIZE) {
        for (int j = 0; j < kJ; j += BLOCK_SIZE) {
            // read blocks c(i,j) into fast memory
            copyBlock(block_c, c, i, j);

            for (int k = 0; k < kK; k += BLOCK_SIZE) {
                // read block a(i,k) into fast memory
                copyBlock(block_a, a, i, k);
                // read block b(i,k) into fast memory
                copyBlock(block_b, b, k, j);

                // matrix multipication
                for (int z = 0; z < BLOCK_SIZE; ++z) {
                    int x, y;
                    #pragma omp parallel for private(x, y)
                    for (x = 0; x < BLOCK_SIZE; ++x) {
                        for (y = 0; y < BLOCK_SIZE; ++y) {
                            // write block c[i][j] back to slow memory
                            block_c[x][y] += block_a[x][z] * block_b[z][y];
                        }
                    }
                }
            }

            // Update block in matrix C
            updateBlock(c, block_c, i, j);
        }
    }
}
