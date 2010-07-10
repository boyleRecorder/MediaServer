#include <stdlib.h>
#ifdef UNIT_TEST
#include <stdio.h>
#endif
#include "DSPAlgorithms.h"

#define ATCK_ENERGY 0.01
#define DECY_ENERGY 0.001

short getChannelEnergy(struct ChannelEnergy *obj, struct bufferData *data)
{
  if(obj != NULL)
  {
    short i;
    float alpha_attack;
    float alpha_decay;
    if(obj->channelBandwidth == BANDWIDTH_8K)
    {
      alpha_attack = ATCK_ENERGY;
      alpha_decay  = DECY_ENERGY;
    }
    else
    {
      alpha_attack = ATCK_ENERGY / 2;
      alpha_decay  = DECY_ENERGY / 2;
    }
    for(i=0;i<data->length;i++)
    {
      short diff;
      diff = abs(data->data[i]) - obj->yn_1;

      if(diff < 0)
      {
        float newEnergy = obj->yn_1 + (float)diff * alpha_decay; 
        obj->yn_1 = (short) newEnergy;
      }
      else 
      {
        float newEnergy = obj->yn_1 + (float)diff * alpha_attack;
        obj->yn_1 = (short) newEnergy;
      }
    }

  }
  return obj->yn_1;
}

struct ChannelEnergy* createChannelEnergyObject(enum BandWidth bw)
{
  struct ChannelEnergy *obj = (struct ChannelEnergy *)malloc(sizeof(struct ChannelEnergy));
  obj->energy = 0;
  obj->yn_1 = 0;
  obj->channelBandwidth = bw;
  return obj;
}

void destroyChannelEnergyObject(struct ChannelEnergy *object)
{
  free(object);
  object = NULL;
}

#ifdef UNIT_TEST

int main()
{
  int i;
  struct ChannelEnergy *object = createChannelEnergyObject(BANDWIDTH_8K);
  struct bufferData data;
  data.data = (short*)malloc(sizeof(data)*100);
  data.length = 100;

  for(i=0;i<data.length;i++)
    data.data[i] = 10000;

  printf("energy: %i\n",getChannelEnergy(object,&data)); 
  for(i=0;i<data.length;i++)
    data.data[i] = 1000;
  for(i=0;i<20;i++)
    printf("energy: %i\n",getChannelEnergy(object,&data)); 
  destroyChannelEnergyObject(object);
  return 0;
}

#endif
