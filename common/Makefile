include ../Makefile.inc
CC=gcc
DEPS =
INCLUDES=-I./include
LIBS=-lm -lpthread -llog4c
OBJ = list.o bufferdata.o 
LIBNAME=libcommon.dylib

%.o: ./src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDES)

test: $(OBJ)
	gcc -dynamiclib -dylib -o ../$(LIBNAME) $^ $(CFLAGS) $(LIBS)

clean:
	rm *.o ../$(LIBNAME)
