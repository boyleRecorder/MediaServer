#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include "filewrite.h"


short createData()
{
  static int offset = 0;
  int frequency = 440;
  float FREQUENCY = 8000.0;

  short value = 30000*sin(2.0*M_PI*((double)(offset))*(double)(frequency)/FREQUENCY);
  offset ++;

return value;
}

int main()
{
  printf("Begin\n");
  struct bufferData data;
  short *array = (short*) malloc(sizeof(short)*160);
  data.data = array;
  data.length = 160;

  int i;
  for(i=0;i<160;i++)
    array[i] = createData();

  initialiseFileWriting();
  FileWriteHandle handle;
  /*
  FILE *stream = fopen("test.wav","w");

  struct WavSink *sink = createWavSink(1,PCM);

  int j;
  for(j=0;j<100;j++)
  {
    for(i=0;i<160;i++)
      array[i] = createData();

    WAV_WriteData(sink,stream,&data);
  }
  closeWavSink(sink,stream);
  */

  handle = getNewHandle("test.wav");
 
  int j;
  for(j=0;j<200;j++)
  {
    for(i=0;i<160;i++)
    {
      array[i] = createData();
    }
  
    writeDataChunk(handle,&data);
  }
//  writeDataChunk(handle,&data);
//  writeDataChunk(handle,&data);

printf("Closing handle.\n");
  closeFileWriteHandle(handle);
  printf("begin sleeps.\n");
  sleep(1);

  sleep(1);
  printf("End\n");



  return EXIT_SUCCESS;
}
