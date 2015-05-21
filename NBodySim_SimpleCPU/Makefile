OS := $(shell uname -s)
ifeq ($(OS),Linux)
LFLAGS = -lm
endif

PLATFORMTARG = "this machine"
USE_OMP = "true"

MYCOMPILER = $(CXX)
ifeq ($(PLATFORMTARG),android)
	ANDRBUILDPATH=/mywork/androidndk-jason-standalone-armandroid19/bin
	MYCOMPILER = $(ANDRBUILDPATH)/arm-linux-androideabi-g++
endif

MYOMPFLAGS = -DUSE_OMP -fopenmp
ifeq ($(USE_OMP),false)
	MYOMPFLAGS = 
endif

all: nbodycpp
nbodycpp: nbodycpp.cpp
	@echo "compiling for: " $(PLATFORMTARG) " using compiler: " $(MYCOMPILER)
	$(MYCOMPILER) nbodycpp.cpp -o nbodycpp $(MYOMPFLAGS) $(LFLAGS)

clean:
	-rm nbodycpp
