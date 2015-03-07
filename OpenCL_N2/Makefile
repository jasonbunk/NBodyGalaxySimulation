OS := $(shell uname -s)
ifeq ($(OS),Linux)
CXX = g++
DEPS = ListOpenCLDeviceInfo.h OpenCLComputationClasses.h
OBJS = ListOpenCLDeviceInfo.o OpenCLComputationClasses.o OpenCL_nbody_n2.o
CXXFLAGS = -Wall -Ipath-OpenCL-include -std=c++11
LFLAGS = -Wall -Lpath-OpenCL-libdir -lOpenCL
else
CXX = clang++
DEPS = ListOpenCLDeviceInfo.h OpenCLComputationClasses.h
OBJS = ListOpenCLDeviceInfo.o OpenCLComputationClasses.o OpenCL_nbody_n2.o
CXXFLAGS = -Wall -Ipath-OpenCL-include -std=c++11 -framework OpenCL
LFLAGS = -Wall -framework OpenCL
endif

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

nbodyocl: $(OBJS)
	$(CXX) -o $@ $^ $(LFLAGS)

clean:
	\rm *.o nbodyocl
