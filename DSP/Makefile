include ../Makefile.inc
CC=gcc
DEPS =
INCLUDES=-I./include -I../common/include
LIBS=-lm -lpthread -llog4c
OBJ = DSPAlgorithms.o bandwidthcontrol.o jitterbuffer.o 
LIBNAME=libdsp.dylib

%.o: ./src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDES)

test: $(OBJ)
	gcc -dynamiclib -dylib -o ../$(LIBNAME) $^ $(CFLAGS) $(LIBS)

clean:
	rm *.o ../$(LIBNAME)
