
/**
  @file DSPAlgorithms.h
  @ingroup DSP
  \brief Some audio DSP algorithms. 
*/
#ifndef DSPALGORITHMS_H__
#define DSPALGORITHMS_H__

#include "bandwidthcontrol.h"
#include "bufferdata.h"

/**
  \struct ChannelEnergy
  \brief The current energy of the channel.
*/
struct ChannelEnergy
{
  /// The current energy of the signal
  short energy;
  /// Used for histeriesious
  short yn_1;
  /// The channel's bandwidth
  enum BandWidth channelBandwidth;
};

/**
  \fn short getChannelEnergy(struct ChannelEnergy *, struct bufferData *data)
  Calculate the updated energy for the line using the new 'data'

  This creates a moving average of the audio level with a history of approximately
  100ms
*/
short getChannelEnergy(struct ChannelEnergy *, struct bufferData *data);

/**
  \fn  struct ChannelEnergy* createChannelEnergyObject(enum BandWidth bw)
  Create a new object for tracking the energy on the line.
*/
struct ChannelEnergy* createChannelEnergyObject(enum BandWidth bw);

/**
  Destroy the channel energy object.
  \fn void destroyChannelEnergyObject(struct ChannelEnergy *object)
*/
void destroyChannelEnergyObject(struct ChannelEnergy *object);


#endif
