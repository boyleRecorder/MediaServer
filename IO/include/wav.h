/**
  @file wav.h
  @ingroup io
  \brief Utility to write wav headers.
*/
#ifndef WAV_H__
#define WAV_H__

/**
 \def WAV_HDR
 \brief The size of a WAV header.
*/
#define WAV_HDR 44

#include "bufferdata.h"
#include "codec.h"

/**
\struct WavSink
\brief A wav header.
*/
struct WavSink
{
  /// The length of raw data to write.
	int  length;
  /// The raw wav header.
	char header[WAV_HDR];
  /// Which codec to use 
  enum CodecList codec; 
};

/**
  Creates a wav header object.
*/
struct WavSink *createWavSink(int channels, int codec);

/**
  Write a block of 'data' to file 'stream'.
  Think of this as a wrapper around fwrite.
*/
void WAV_WriteData(struct WavSink *sink, FILE *stream, struct bufferData *data);

/**
  Writes the header to the file 'stream' and closes the file.
  Destroys memory allocated for 'header'
*/
void closeWavSink(struct WavSink *header, FILE *stream);


#endif
