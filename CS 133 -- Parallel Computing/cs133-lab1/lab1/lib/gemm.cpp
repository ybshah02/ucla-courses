#include <cmath>
#include <cstring>

#include <chrono>
#include <iostream>
#include <random>
#include <vector>

#include "gemm.h"

using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::steady_clock;
using std::clog;
using std::endl;
using std::vector;

void GemmBaseline(const vector<vector<float>>& a,
                  const vector<vector<float>>& b,
                  vector<vector<float>>* c);
void GemmBaseline(const float a[kI][kK], const float b[kK][kJ],
                  float c[kI][kJ]) {
  vector<vector<float>> a_vec(kI);
  vector<vector<float>> b_vec(kK);
  vector<vector<float>> c_vec(kI);
  for (int i = 0; i < kI; ++i) {
    a_vec[i].resize(kK);
    c_vec[i].resize(kJ);
    std::memcpy(a_vec[i].data(), a[i], sizeof(float) * kK);
  }
  for (int k = 0; k < kK; ++k) {
    b_vec[k].resize(kJ);
    std::memcpy(b_vec[k].data(), b[k], sizeof(float) * kJ);
  }
  GemmBaseline(a_vec, b_vec, &c_vec);
  for (int i = 0; i < kI; ++i) {
    std::memcpy(c[i], c_vec[i].data(), sizeof(float) * kJ);
  }
}

void GemmSequential(const float a[kI][kK], const float b[kK][kJ],
                    float c[kI][kJ]) {
  for (int i = 0; i < kI; ++i) {
    std::memset(c[i], 0, sizeof(float) * kJ);
  }
  for (int i = 0; i < kI; ++i) {
    for (int j = 0; j < kJ; ++j) {
      for (int k = 0; k < kK; ++k) {
        c[i][j] += a[i][k] * b[k][j];
      }
    }
  }
}

void Init(float a[kI][kK], float b[kK][kJ]) {
  clog << "Problem size: " << kI << " x " << kK << " x " <<  kJ << endl;

  std::default_random_engine generator(
      steady_clock::now().time_since_epoch().count());
  std::uniform_real_distribution<float> distribution(0.f, 1.f);

  clog << "Initialize matrices a and b\n";
  for (int i = 0; i < kI; ++i) {
    for (int k = 0; k < kK; ++k) {
      a[i][k] = distribution(generator);
    }
  }
  for (int k = 0; k < kK; ++k) {
    for (int j = 0; j < kJ; ++j) {
      b[k][j] = distribution(generator);
    }
  }
}

int Diff(const float c1[kI][kJ], const float c2[kI][kJ]) {
  double diff = 0.;
  auto square = [](double x) -> double { return x * x; };
  for (int i = 0; i < kI; ++i) {
    for (int j = 0; j < kJ; ++j) {
      diff += square(double(c1[i][j]) - c2[i][j]);
    }
  }
  diff /= kI * kJ;
  if (! std::isfinite(diff) || diff > 1e-5) {
    clog << "Diff: " << diff << endl;
    return 1;
  }
  return 0;
}

void Benchmark(
    void (*gemm)(const float[kI][kK], const float[kK][kJ], float[kI][kJ]),
    const float a[kI][kK], const float b[kK][kJ], float c[kI][kJ]) {
  const auto begin = steady_clock::now();
  (*gemm)(a, b, c);
  const auto end = steady_clock::now();
  uint64_t run_time_us = duration_cast<microseconds>(end - begin).count();
  float gflops = 2.0 * kI * kJ * kK / (run_time_us * 1e3);
  clog << "Time: " << run_time_us * 1e-6 << " s\n";
  clog << "Perf: " << gflops << " GFlops\n";
}
