/**
 @file bandwidthcontrol.h
 \brief Functions to Up/Down samples audio data from one bandwidth to another.
 */

#ifndef BANDWIDTHCONTROL_H__
#define BANDWIDTHCONTROL_H__

#include "bufferdata.h"

/**
  Bandwidth enumerator.
  Lists the supported bandwidths. 
  */
enum BandWidth{
   /// 8Khz audio
  BANDWIDTH_8K,
  /// 16 Khz
  BANDWIDTH_16K
};

/**
  \fn void upSampleData(struct bufferData *lowFreq, struct bufferData *highFreq);
  \brief up samples a low frequence to a higher one.
  \param highFreq The lenght returned in this is equal to twice the length of 'lowFreq'.
  \warning It is assumed that sufficient memory is allocated for these containers.
  */
void upSampleData(struct bufferData *lowFreq, struct bufferData *highFreq);

/**
  \fn void downSampleData(struct bufferData *highFreq, struct bufferData *lowFreq)
  \brief down samples a high frequence to a lower one.
\param highFreq The lenght of this must be an even integer.
  \warning It is assumed that sufficient memory is allocated for these containers.
  */
void downSampleData(struct bufferData *highFreq, struct bufferData *lowFreq);


#endif
