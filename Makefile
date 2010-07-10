CC=gcc
CFLAGS=-I. -g -ggdb -Wall -O0 -fstack-protector -fno-stack-protector -DWRITE_UNIT_TEST
DEPS =
LIBS=-lm -lpthread -llog4c
OBJ = list.o jitterbuffer.o filewrite.o bufferdata.o
EXEC=test

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm *.o
