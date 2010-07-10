
/** 
  @file filewrite.h
  \brief  An async file writing library 

  A file writing mechanism that will never block.
 */ 

#ifndef FILEWRITE_H__
#define FILEWRITE_H__

#include "list.h"
#include "bufferdata.h"

/**
  \struct FileWriteObject
  \brief An async object to be written to file. 
*/
struct FileWriteObject
{
  char *fileName;
  char fileType;
  struct node *writeObjects;
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
