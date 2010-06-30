#ifndef FILEWRITE_H__
#define FILEWRITE_H__

struct FileWriteObject
{
  char *fileName;
  char fileType;

};

typedef void  FileWriteHandle

FileWriteHandle* getNewHandle(char *fileName);

void initialiseFileWriting();

void closeFileWriteHandle(FileWriteHandle* handle);

void writeDataChunk(FileWriteHandle *handle,struct bufferData *data); 

#endif
