#include <stdlib.h>
#include <stdio.h>

#include "filewrite.h"

int main()
{
  printf("Begin\n");
  struct bufferData data;
  short *array = (short*) malloc(sizeof(short)*160);
  data.data = array;
  data.length = 160;


  initialiseFileWriting();
  FileWriteHandle handle;

  handle = getNewHandle("test.wav");
  
  writeDataChunk(handle,&data);
  writeDataChunk(handle,&data);
  writeDataChunk(handle,&data);

  sleep(1);
  closeFileWriteHandle(handle);

  sleep(1);
  printf("End\n");



  return EXIT_SUCCESS;
}
