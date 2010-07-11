
/** 
  @file filewrite.h
  @ingroup io 
  \brief  An async file writing library 

  A file writing mechanism that will never block.

  This writing mechanism setups a separate thread to do all the 
  file writing. The thread has a list, and the data is pushed
  onto the list.
 */ 

#ifndef FILEWRITE_H__
#define FILEWRITE_H__

#include "list.h"
#include "bufferdata.h"
#include "wav.h"

/**
  \struct FileWriteObject
  \brief An async object to be written to file. 
*/
struct FileWriteObject
{
  /// The name of the file.
  char *fileName;
  /// The wav header.
  struct WavSink *wav;
  /// The data objects which are to be written.
  struct node *writeObjects;
  /// The file itself.
  FILE *stream;
  /// 0, not yet opened, 1 opened for writing, -1 opening failed.
  char fileStatus;
};

/**
  A handle for a write object.
  */
typedef void *  FileWriteHandle;

/**
  \fn FileWriteHandle getNewHandle(char *fileName)
  \param fileName the name of the file to open
  This function will not block.
  */
FileWriteHandle getNewHandle(char *fileName);

/**
  \fn void initialiseFileWriting()
  Initializes the file writing. Must be called before doing any file write.
  */
void initialiseFileWriting();

/**
  \fn void closeFileWriteHandle(FileWriteHandle handle)
  Closes the file write handle. 
  */
void closeFileWriteHandle(FileWriteHandle handle);

/**
  \fn int writeDataChunk(FileWriteHandle handle, struct bufferData *data)
  Writes a block of data to the file handle. This will not block.
  \param handle The file write handle.
  \param data The block of data that is to be written.
  */
int writeDataChunk(FileWriteHandle handle, struct bufferData *data); 

#endif
