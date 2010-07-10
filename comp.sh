#!/bin/bash
gcc bufferdata.c -c -g -ggdb -Wall -fstack-protector -fno-stack-protector
gcc list.c -c -g -ggdb -Wall -fstack-protector -fno-stack-protector
gcc jitterbuffer.c -c -g -ggdb -Wall -fstack-protector -fno-stack-protector
gcc filewrite.c -c -g -ggdb -Wall -DUNIT_TEST -fstack-protector -fno-stack-protector
gcc filewrite.o list.o bufferdata.o -lm -lpthread  -fstack-protector -fno-stack-protector
#gcc main.c -c -g -ggdb -Wall 
#gcc main.o jitterbuffer.o -Wall -g -ggdb -llog4c -DDEBUGGING 
