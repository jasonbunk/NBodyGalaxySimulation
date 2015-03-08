OS := $(shell uname -s)
ifeq ($(OS),Linux)
LFLAGS = -lm
endif

all: aarseth
aarseth: nbody0-lab.c
	$(CC) nbody0-lab.c -o aarseth $(LFLAGS)

clean:
	\rm aarseth
