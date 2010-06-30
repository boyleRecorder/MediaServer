
#include <assert.h>

#include "bandwidthcontrol.h"

void upSampleData(struct bufferData *lowFreq, struct bufferData *highFreq)
{
  unsigned short i;

  assert(lowFreq->length > 2);

  for(i=0;i<lowFreq->length-1;i++)
  {
    highFreq->data[i<<1] = (lowFreq->data[i] + lowFreq->data[i+1]) >> 1; 
  }
  highFreq[(lowFreq->length >> 1) -1] = lowFreq[lowFreq->length-1];


}

void downSampleData(struct bufferData *highFreq, struct bufferData *lowFreq)
{
  unsigned short i;
  unsigned short length = highFreq->length >> 1;

  for(i=0;i<length;i++)
    lowFreq->data[i] = highFreq->data[i<<1];

  lowFreq->length = length;


}

