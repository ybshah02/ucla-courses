ifneq (,$(wildcard /usr/local/opt/llvm/bin/clang++))
  CXX = /usr/local/opt/llvm/bin/clang++
  MPICH_CXX = /usr/local/opt/llvm/bin/clang++
  CXXFLAGS += -I/usr/local/opt/llvm/include
  LDFLAGS  += -L/usr/local/opt/llvm/lib -lomp
  LAB2_CXX_FLAGS = -L/usr/local/opt/boost/lib -I/usr/local/opt/boost/include
else ifneq (,$(wildcard /opt/homebrew/opt/llvm/bin/clang++))
  CXX = /opt/homebrew/opt/llvm/bin/clang++
  MPICH_CXX = /opt/homebrew/opt/llvm/bin/clang++
  CXXFLAGS += -I/opt/homebrew/opt/llvm/include
  LDFLAGS  += -L/opt/homebrew/opt/llvm/lib -lomp
  LAB2_CXX_FLAGS = -L/opt/homebrew/opt/boost/lib -I/opt/homebrew/opt/boost/include
else
  CXX = g++
  MPICH_CXX = g++
  CXXFLAGS += -fno-loop-interchange -fno-loop-unroll-and-jam
  LDFLAGS  += -lgomp
  LAB2_CXX_FLAGS =
endif

CXXFLAGS += -std=c++17 -O3 -fopenmp \
            -march=native $(LDFLAGS)

clang-tidy: $(SRCS)
	clang-tidy $(filter %.cpp, $^) -- -Wall

zip: submission.zip

submission.zip: $(REQUIRED_FILES)
	zip -r submission.zip $^
