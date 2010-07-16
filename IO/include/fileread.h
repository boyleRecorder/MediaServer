#ifndef FILEREAD_H__
#define FILEREAD_H__


#include "list.h"
#include "bufferdata.h"
#include "wav.h"

/**
  \struct FileReadObject
  \brief An async object to read from file. 
*/
struct FileReadObject
{
  /// The name of the file.
  char *fileName;
  /// The wav header.
  struct WavSink *wav;
  /// The data objects which store the read data.
  struct node *readObjects;
  /// The file itself.
  FILE *stream;
  /// 0, not yet opened, 1 opened for writing, 2 eof,  -1 opening failed.
  char fileStatus;

  unsigned readPosition;

  /// headerOpen
  char headerOpen;
};

/**
  A handle for a read object.
  */
typedef void *  FileReadHandle;

/**
  \fn FileReadHandle getNewReadHandle(char *fileName)
  \param fileName the name of the file to open
  This function will not block.
  */
FileReadHandle getNewReadHandle(char *fileName);

/**
  \fn void initialiseFileRead()
  Initializes the file reading mechanism. Must be called before doing any file read.
  */
void initialiseFileRead();

/**
  \fn void closeFileReadHandle(FileReadHandle handle)
  Closes the file read handle. 
  */
void closeFileReadHandle(FileReadHandle handle);

/**
  \fn int readDataChunk(FileReadHandle handle, struct bufferData *data)
  Reads a block of data to the file handle. This will not block.
  
  \param handle The file write handle.
  \param data The structure that the date read is to be put. data->length is the amount that will be read.

  \warning: Memory has to be allocated for 'data' before calling this.
  */
int readDataChunk(FileReadHandle handle, struct bufferData *data); 


#endif
