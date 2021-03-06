include Makefile.inc
CC=gcc
DIRS=common
DEPS =
DEFINES=
LIBS=-lm -lpthread -llog4c 
INCLUDES=-I./common/include -I./DSP/include -I./IO/include
OBJ = main.o 
EXEC=test
OBJLIBS=libcommon.dylib libdsp.dylib libio.dylib


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

test: $(OBJ) $(OBJLIBS) 
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

libcommon.dylib:
	cd common; $(MAKE) $(MFLAG) 

libdsp.dylib:
	cd DSP; $(MAKE) $(MFLAG) 


libio.dylib:
	cd IO; $(MAKE) $(MFLAG) 


clean:
	rm *.o $(EXEC) ; cd common ; $(MAKE) $(MFLAG) clean
	cd DSP ; $(MAKE) $(MFLAG) clean ; cd ../IO ; $(MAKE) $(MFLAG) clean
