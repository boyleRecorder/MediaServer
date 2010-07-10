include ../Makefile.inc
CC=gcc
DEPS =
INCLUDES=-I./include -I../common/include
LIBS=-lm -lpthread -llog4c -L../ -lcommon
OBJ = filewrite.o
DEFINES=
LIBNAME=libio.dylib

%.o: ./src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDES) $(DEFINES)

test: $(OBJ)
	gcc -dynamiclib -dylib -o ../$(LIBNAME) $^ $(CFLAGS) $(LIBS)

clean:
	rm *.o ../$(LIBNAME)
