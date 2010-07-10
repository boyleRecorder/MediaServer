include Makefile.inc
CC=gcc
#CFLAGS=-I. -g -ggdb -Wall -O0 -fstack-protector -fno-stack-protector -DWRITE_UNIT_TEST
DIRS=common
DEPS =
DEFINES=-DWRITE_UNIT_TEST
LIBS=-lm -lpthread -llog4c 
INCLUDES=-I./common/include
OBJ = jitterbuffer.o filewrite.o DSPAlgorithms.o
EXEC=test
OBJLIBS=libcommon.dylib


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

test: $(OBJ) $(OBJLIBS) 
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

libcommon.dylib:
	cd common; $(MAKE) $(MFLAG) 


clean:
	rm *.o rm $(EXEC)
	cd common ; $(MAKE) $(MFLAG) clean
