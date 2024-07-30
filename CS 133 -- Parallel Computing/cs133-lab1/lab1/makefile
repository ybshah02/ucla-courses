#CXX = g++  # uncomment and specify your compiler here
LDFLAGS +=  # specify your library linking options here
SRCS = lib/gemm.h lib/gemm.cpp lib/main.cpp omp.cpp omp-blocked.cpp

ifeq ($(shell uname -m),arm64)
    SRCS += lib/gemm-baseline-aarch64-g++.a lib/gemm-baseline-aarch64-llvm.a
else
	ifeq ($(shell uname -s),Darwin)
		SRCS += lib/gemm-baseline-darwin-g++.a lib/gemm-baseline-darwin-llvm.a
	else
		SRCS += lib/gemm-baseline-linux-g++.a
	endif
endif

REQUIRED_FILES = omp.cpp omp-blocked.cpp lab1-report.pdf

test: gemm
	./$^ parallel parallel-blocked

gemm: $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(filter %.cpp %.a, $^)

clean:
	$(RM) gemm

include ../common/makefile.inc
