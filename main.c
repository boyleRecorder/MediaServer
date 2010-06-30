#include <stdlib.h>
#include <stdio.h>


#include "jitterbuffer.h"

/**
  Creates a dummy rtp packet.
  */
void incrementPacket(JitterBufferHandle *handle, struct ReceivedPacket *packet)
{
  packet->markerBit = 0;
  packet->sequenceNumber += 1;
  packet->timeStamp += 160;
}

void printJitterBuffer(JitterBufferHandle *handle )
{
  unsigned short i;
  FILE *stream = fopen("buffer.dat","w");
  for(i=0;i<0x800;i++)
  {

    fprintf(stream,"%i\n",handle->jitterBufferData[i]);
  }
  fclose(stream);

}

/**
  Writes a block of dta to file.
  */
void writeExtractedData(struct bufferData *data)
{
  static int first = 0;
  FILE *stream;
  if(first == 0) // Wipe the file the first time it is opened.
  {
    stream = fopen("extracted.dat","w");
    first = 1;
  }
  else 
  {
    stream = fopen("extracted.dat","a");
  }

  unsigned i;
  for(i=0;i<data->length;i++)
  {
    fprintf(stream,"%i\n",data->data[i]);
  }

  fclose(stream);

}


void runTest(JitterBufferHandle *handle)
{
  struct bufferData data;
  struct bufferData extractedData;

  data.data = (short *)malloc(sizeof(short)*160);
  data.length = 160;

  extractedData.data = (short *)malloc(sizeof(short)*160);
  extractedData.length = 160;

  struct ReceivedPacket packet;
  packet.markerBit = 0;
  packet.sequenceNumber = 1;
  packet.timeStamp = 80;
  packet.data = &data;

  unsigned int i;
  for(i=0;i<160;i++)
    packet.data->data[i] = 1;

  setPacketSize(handle,8);

  
  // First packet.
  initializeBuffer(handle);
  processReceivedPacket(handle,&packet);
  extractData(handle,&extractedData);
 
  // A normal packet 
  incrementPacket(handle,&packet);
  processReceivedPacket(handle,&packet);
  extractData(handle,&extractedData);


  // A dropped packet.
  extractData(handle,&extractedData);

  incrementPacket(handle,&packet);
  incrementPacket(handle,&packet);
  processReceivedPacket(handle,&packet);
  extractData(handle,&extractedData);

  // A series of normal packet 
  for (i=0;i<5;i++) {
    incrementPacket(handle,&packet);
    processReceivedPacket(handle,&packet);
    extractData(handle,&extractedData);
    writeExtractedData(&extractedData);
  }

  // A sequence of no packets received, i.e. silent suppression.
  // Jitter buffer should be reset after a little time.
  for (i=0;i<6;i++) {
    incrementPacket(handle,&packet);
    extractData(handle,&extractedData);
    writeExtractedData(&extractedData);
  }

  // A series of normal packet after a marker bit. 
  for (i=0;i<5;i++) {
    incrementPacket(handle,&packet);
    
    if(i==0)
      packet.markerBit = 1;
    else
      packet.markerBit = 0;

    processReceivedPacket(handle,&packet);
    extractData(handle,&extractedData);
    writeExtractedData(&extractedData);
  }

  // A duplicate packet has been received. 
  processReceivedPacket(handle,&packet);
  extractData(handle,&extractedData);
  writeExtractedData(&extractedData);

  // A series of normal packets
  for (i=0;i<5;i++) {
    incrementPacket(handle,&packet);
    processReceivedPacket(handle,&packet);
    extractData(handle,&extractedData);
    writeExtractedData(&extractedData);
  }



  free(data.data);
  free(extractedData.data);
}

int main(int argc, char **argv)
{

  JitterBufferHandle *handle;
  handle = createJitterBuffer(1);
  setupJitterBufferLogger(NULL);

  runTest(handle);

  printJitterBuffer(handle);
  destroyJitterBuffer(handle);

  return EXIT_SUCCESS;
}
