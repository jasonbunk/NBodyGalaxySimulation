OS := $(shell uname -s)
ifeq ($(OS),Linux)
LFLAGS = -lm
endif

all: nbodycpp
nbodycpp: nbodycpp.cpp
	$(CXX) nbodycpp.cpp -o nbodycpp $(LFLAGS)

clean:
	\rm nbodycpp
