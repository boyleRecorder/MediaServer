#ifndef BUFFER_DATA_H__
#define BUFFER_DATA_H__

/**
 @file bufferdata.h
 \brief A block of data.
 */

/**
  \struct bufferData
  \brief A block of data.
  A block of data to be put into, or removed from the buffer.

  */
struct bufferData
{
  /// An array of data. 
  short *data;
  /// The length of the 'data' pointer.
  short length;
};

#endif
