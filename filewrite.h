#ifndef FILEWRITE_H__
#define FILEWRITE_H__

#include "bufferdata.h"

struct FileWriteObject
{
  char *fileName;
  char fileType;
  struct node *writeObjects;
  FILE *stream;
  /// 0, not yet opened, 1 opened for writing, -1 opening failed.
  char fileStatus;
};

typedef void *  FileWriteHandle;

FileWriteHandle getNewHandle(char *fileName);

void initialiseFileWriting();

void closeFileWriteHandle(FileWriteHandle handle);

int writeDataChunk(FileWriteHandle handle, struct bufferData *data); 

#endif
