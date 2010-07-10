
/** 
  @file jitterbuffer.h
  @ingroup DSP
  \brief The jitter buffer

  This is a dynamic jitter buffer for RTP.
 */ 

#ifndef JITTERBUFFER_H__
#define JITTERBUFFER_H__

#include "bufferdata.h"

////////////////////////
// structures.
////////////////////////

/** 
  \struct ReceivedPacket
  \brief A reduced version of a RTP packet.
  */
struct ReceivedPacket
{
  /// Must be decoded data.
  struct bufferData *data;
  /// RTP sequence number
  unsigned short sequenceNumber;
  /// RTP time stamp
  unsigned int timeStamp;

  /// The rtp marker bit
  char markerBit;
};

/**
  \struct JitterBuffer
  \brief The jitter buffer state.

  This contains the jitter buffer data and all the state information
  required for the jitter buffer to operate properly.

  The jitter buffer is dynamical depending on the jitter. The following feature should 
  be supported.

  - Silent suppression.
  - Marker bit.
  - Packet reordering. 
  - Dynamical jitter support


  */
struct JitterBuffer
{
  /// sequence number of previous received packet.
  unsigned short previousSequenceNumber;
  /// Time stamp of previous received packet.
  unsigned int   previousTimeStamp;

  /// Set to 1 on initialization. Indicates the first RTP packet.
  unsigned char  firstPacket;

  /// The highest sequence number received to data. 
  unsigned short maxSeqNumber;

  /// The number of points in 1 ms of data; codec dependent.
  unsigned short packetSize;

  /// The estimated jitter for the line.
  unsigned short jitter;

  /// The pointer in the jitter buffer to the received data.
  unsigned short rxPtr;
  /// The pointer in the jitter buffer to the data to be transmitted.
  unsigned short txPtr;

  /// The jitter buffer itself.
  short *jitterBufferData;


  // These are used to control the resizing of the jitter buffer.
  /// The average size of the buffer over the last time period.
  unsigned short averageBufferSize;

  /// The time since the last average was performed.
  unsigned short averageCounter;

  /// An array which contains a history of the previous sizes of the jitter buffer.
  unsigned short *runningAverageArray;

  /// A flag to indicate that the jitter buffer should be resized.
  char resizing;

  /// Channel ID
  unsigned chanId;
};


/*!
  \var typedef struct JitterBuffer JitterBufferHandle 
  \brief The jitter buffer structure.
          
*/

typedef struct JitterBuffer JitterBufferHandle ;


/**
  \fn JitterBufferHandle * createJitterBuffer(unsigned chanId)
  \brief Creates a new jitter buffer.
  \param chanId The channel ID.
  */
JitterBufferHandle * createJitterBuffer(unsigned chanId);

/**
  \fn void destroyJitterBuffer(JitterBufferHandle *)
  \brief destroys an existing jitter buffer.
  */
void destroyJitterBuffer(JitterBufferHandle *);

void setStreamJitter(JitterBufferHandle *handle, unsigned short jitter);
void setPacketSize(JitterBufferHandle *handle, unsigned short);

void initializeBuffer(JitterBufferHandle *handle );

void processReceivedPacket(JitterBufferHandle *handle, struct ReceivedPacket *packet);
void extractData(JitterBufferHandle *handle, struct bufferData *data);

/**
  \fn void setupJitterBufferLogger(char *logger)
  \brief Setups the log4c logger.

  If 'logger' is NULL then a logger by the name of 
  'prototype.log.app.jitterbuffer' is used.

  \param logger The name of the logger to setup.
  */
void setupJitterBufferLogger(char *logger);

#endif
