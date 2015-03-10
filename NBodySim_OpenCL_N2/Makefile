
PLATFORMTARG = "this machine"

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
CXXFLAGS = -Wall -Ipath-OpenCL-include
LFLAGS = -Wall -Lpath-OpenCL-libdir -lOpenCL
else
MYCOMPILER = clang++
CXXFLAGS = -Wall -Ipath-OpenCL-include -framework OpenCL
LFLAGS = -Wall -framework OpenCL
endif

endif

DEPS = ListOpenCLDeviceInfo.h OpenCLComputationClasses.h
OBJS = ListOpenCLDeviceInfo.o OpenCLComputationClasses.o OpenCL_nbody_n2.o

%.o: %.cpp $(DEPS)
	@echo "compiling for: " $(PLATFORMTARG) " using compiler: " $(MYCOMPILER)
	$(MYCOMPILER) -c -o $@ $< $(CXXFLAGS)

nbodyocl: $(OBJS)
	@echo "compiling for: " $(PLATFORMTARG) " using compiler: " $(MYCOMPILER)
	$(MYCOMPILER) -o $@ $^ $(LFLAGS)

clean:
	\rm *.o nbodyocl
