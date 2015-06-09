OS := $(shell uname -s)
ifeq ($(OS),Linux)
LFLAGS = -lm
endif

PLATFORMTARG = "this machine"

MYCOMPILER = mpicxx


all: nbodycpumpi
nbodycpumpi: nbodycpumpi.cpp
	@echo "compiling for: " $(PLATFORMTARG) " using compiler: " $(MYCOMPILER)
	$(MYCOMPILER) nbodycpumpi.cpp -o nbodycpumpi $(LFLAGS)

clean:
	-rm nbodycpumpi
