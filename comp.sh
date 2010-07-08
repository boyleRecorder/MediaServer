#!/bin/bash
gcc bufferdata.c -c -g -ggdb -Wall
gcc list.c -c -g -ggdb -Wall
gcc jitterbuffer.c -c -g -ggdb -Wall
gcc filewrite.c -c -g -ggdb -Wall -DUNIT_TEST
gcc filewrite.o list.o bufferdata.o -lm -lpthread 
#gcc main.c -c -g -ggdb -Wall 
#gcc main.o jitterbuffer.o -Wall -g -ggdb -llog4c -DDEBUGGING 
