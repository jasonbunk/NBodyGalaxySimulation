OS := $(shell uname -s)
ifeq ($(OS),Linux)
LFLAGS = -lm
endif

PLATFORMTARG = "this machine"
USE_OMP = "true"

MYCOMPILER = mpicxx

MYOMPFLAGS = -DUSE_OMP -fopenmp
ifeq ($(USE_OMP),false)
	MYOMPFLAGS = 
endif

all: nbodycpumpi
nbodycpumpi: nbodycpumpi.cpp
	@echo "compiling for: " $(PLATFORMTARG) " using compiler: " $(MYCOMPILER)
	$(MYCOMPILER) nbodycpumpi.cpp -o nbodycpumpi $(MYOMPFLAGS) $(LFLAGS)

clean:
	-rm nbodycpumpi
