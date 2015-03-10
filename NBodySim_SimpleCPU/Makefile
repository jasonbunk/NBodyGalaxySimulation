OS := $(shell uname -s)
ifeq ($(OS),Linux)
LFLAGS = -lm
endif

PLATFORMTARG = "this machine"

MYCOMPILER = $(CXX)
ifeq ($(PLATFORMTARG),android)
	ANDRBUILDPATH=/mywork/androidndk-jason-standalone-armandroid19/bin
	MYCOMPILER = $(ANDRBUILDPATH)/arm-linux-androideabi-g++
endif

all: nbodycpp
nbodycpp: nbodycpp.cpp
	@echo "compiling for: " $(PLATFORMTARG) " using compiler: " $(MYCOMPILER)
	$(MYCOMPILER) nbodycpp.cpp -o nbodycpp $(LFLAGS)

clean:
	\rm nbodycpp
