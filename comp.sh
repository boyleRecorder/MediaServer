#!/bin/bash

gcc jitterbuffer.c -c -g -ggdb -Wall
gcc main.c -c -g -ggdb -Wall 
gcc main.o jitterbuffer.o -Wall -g -ggdb -llog4c -DDEBUGGING 
