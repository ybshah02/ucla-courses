#include <iostream>

#include <mpi.h>

#include "gemm.h"

using std::clog;
using std::endl;

int main(int argc, char** argv) {
  int rank;
  const int kRoot = 0;
  float (*a)[kK] = nullptr;
  float (*b)[kJ] = nullptr;
  float (*c)[kJ] = nullptr;
  float (*c_base)[kJ] = nullptr;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == kRoot) {
    a = new float[kI][kK];
    b = new float[kK][kJ];
    c = new float[kI][kJ];
    c_base = new float[kI][kJ];

    Init(a, b);

    GemmBaseline(a, b, c_base);

    clog << "\nRun parallel GEMM with MPI\n";
  }

  MPI_Barrier(MPI_COMM_WORLD);
  double begin = MPI_Wtime();
  GemmParallelBlocked(a, b, c);
  MPI_Barrier(MPI_COMM_WORLD);
  double end = MPI_Wtime();

  if (rank == kRoot) {
    double run_time = end - begin;
    float gflops = 2.0 * kI * kJ * kK / (run_time * 1e9);
    clog << "Time: " << run_time << " s\n";
    clog << "Perf: " << gflops << " GFlops\n";

    bool fail = false;
    if (Diff(c_base, c) != 0) {
      fail = true;
    }

    delete[] a;
    delete[] b;
    delete[] c;
    delete[] c_base;

    if (fail) {
      clog << "Your answer is INCORRECT!\n";
      return 1;
    }
  }

  MPI_Finalize();
  return 0;
}
