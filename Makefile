include Makefile.inc
CC=gcc
DIRS=common
DEPS =
DEFINES=-DWRITE_UNIT_TEST
LIBS=-lm -lpthread -llog4c 
INCLUDES=-I./common/include
OBJ = filewrite.o 
EXEC=test
OBJLIBS=libcommon.dylib libdsp.dylib


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

test: $(OBJ) $(OBJLIBS) 
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

libcommon.dylib:
	cd common; $(MAKE) $(MFLAG) 

libdsp.dylib:
	cd DSP; $(MAKE) $(MFLAG) 


clean:
	rm *.o $(EXEC) ; cd common ; $(MAKE) $(MFLAG) clean
	cd DSP ; $(MAKE) $(MFLAG) clean
