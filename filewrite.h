
struct FileWriteObject
{
  char *fileName;
  char fileType;

};

typedef void  FileWriteHandle

FileWriteHandle* getNewHandle(char *fileName);

void closeFileWriteHandle(FileWriteHandle* handle);

void writeDataChunk(FileWriteHandle *handle,struct bufferData *data); 


