CXX = nvcc
CXXFLAGS += -std=c++11 -O3

CNN_SRCS = lib/cnn.cuh \
	lib/utils.cuh lib/utils.cu \
	lib/cnn_seq.cuh lib/cnn_seq.cu \
	cnn_gpu.cuh cnn_gpu.cu \
	lib/main.cu
VADD_SRCS = lib/utils.cuh lib/utils.cu lib/vadd.cu

test: cnn
	. ./params.sh; ./$<

test-seq: cnn
	@$(MAKE) --no-print-directory test SEQUENTIAL=

cnn: $(CNN_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(filter %.cu %.a %.o, $^)

test-vadd: vadd
	. ./params.sh; ./$<

vadd: $(VADD_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(filter %.cu %.a %.o, $^)

clean:
	$(RM) vadd cnn