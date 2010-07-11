#include <string.h>
#include <stdlib.h>

#include "bufferdata.h"


struct bufferData* deepCopyBufferData(struct bufferData *data)
{
  struct bufferData *newObject = (struct bufferData *)malloc(sizeof(struct bufferData)); 
  newObject->data = NULL;
  newObject->length = 0;

  if(data != NULL)
  {
    if(data->length > 0)
    {
      newObject->data = (short*)malloc(sizeof(short)*data->length);
      newObject->length = data->length;
      memcpy(newObject->data,data->data,data->length*sizeof(short));

      
    }
  }

  return newObject;
}
