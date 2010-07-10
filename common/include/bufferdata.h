#ifndef BUFFER_DATA_H__
#define BUFFER_DATA_H__

/**
 @file bufferdata.h
  @ingroup common
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

/**
  \fn struct bufferData* deepCopyBufferData(struct bufferData *data)
  Does a deep copy of 'data' and returns a copy of it.
  New data is allocated for the returned object.
  */
struct bufferData* deepCopyBufferData(struct bufferData *data);

#endif
