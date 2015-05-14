
PLATFORMTARG = "this machine"


CL_HPP_HEADER_CHECK := $(strip $(wildcard CL/cl.hpp))
ifeq ($(CL_HPP_HEADER_CHECK),)
	CL_HPP_HEADER = "-DUSE_HEADER_CL12_FOR_CPP"
else
	CL_HPP_HEADER = ""
endif


MYCOMPILER = $(CXX)
ifeq ($(PLATFORMTARG),android)
	ANDRBUILDPATH=/mywork/androidndk-jason-standalone-armandroid19/bin
	MYCOMPILER = $(ANDRBUILDPATH)/arm-linux-androideabi-g++
	
	ROOT:=../..
	CXXFLAGS = -Wall -I$(ROOT)/include -I$(ROOT)/common -I. -D__ANDROID_MALI_OPENCL_
	CFLAGS =   -Wall -I$(ROOT)/include -I$(ROOT)/common -I. -D__ANDROID_MALI_OPENCL_
	LFLAGS = -Wall -L$(ROOT)/lib -L$(ROOT)/common -lOpenCL -lCommon
else
OS := $(shell uname -s)
ifeq ($(OS),Linux)
	MYCOMPILER = g++
	CXXFLAGS = -Wall
	LFLAGS = -Wall -lOpenCL
else
	MYCOMPILER = clang++
	CXXFLAGS = -Wall -framework OpenCL
	LFLAGS = -Wall -framework OpenCL
endif
endif


DEPS = ListOpenCLDeviceInfo.h OpenCLComputationClasses.h
OBJS = ListOpenCLDeviceInfo.o OpenCLComputationClasses.o OpenCL_nbody_n2.o

%.o: %.cpp $(DEPS)
	@echo "INCLUDE_PATH == " $(INCLUDE_PATH)
	@echo "compiling for: " $(PLATFORMTARG) " using compiler: " $(MYCOMPILER)
	$(MYCOMPILER) -c -o $@ $< $(CXXFLAGS) $(CL_HPP_HEADER)

nbodyocl: $(OBJS)
	@echo "linking for: " $(PLATFORMTARG) " using compiler: " $(MYCOMPILER)
	$(MYCOMPILER) -o $@ $^ $(LFLAGS)

clean:
	\rm *.o nbodyocl

