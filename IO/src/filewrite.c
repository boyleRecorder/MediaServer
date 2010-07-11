#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>

#include "list.h"
#include "filewrite.h"

static struct node *fileOpenList;
static struct node *fileWriteList;
static struct node *fileCloseList;

static pthread_mutex_t writeMutex = PTHREAD_MUTEX_INITIALIZER;

static void addToWritePendingList(struct FileWriteObject *object)
{
  pthread_mutex_lock(&writeMutex);
  pushData(fileWriteList,(void*)object);
  pthread_mutex_unlock(&writeMutex);
}

static void openPendingFiles()
{
  struct node *listObject;
  struct FileWriteObject *object;
  pthread_mutex_lock(&writeMutex);
  if(listSize(fileOpenList) > 0)
  {
    listObject = popData(fileOpenList);
    pthread_mutex_unlock(&writeMutex);
    object = (struct FileWriteObject*)listObject->data;
    destroyList(listObject);

    // Warning: this can block.
    FILE *stream = fopen(object->fileName,"w");
    if(stream != NULL)
    {
      printf("Stream opened:\n");
      object->stream = stream;

      pthread_mutex_lock(&writeMutex);
      object->fileStatus = 1;
      pthread_mutex_unlock(&writeMutex);
      addToWritePendingList(object);

    }
    else
    {
      object->fileStatus = -1;	
      pthread_mutex_unlock(&writeMutex);
    }
  }
  else
    pthread_mutex_unlock(&writeMutex);

}

static void writeFiles()
{
  int i;
  int size = listSize(fileWriteList);
  for(i=0;i<size;i++)
  {
    struct FileWriteObject *object = getElement(fileWriteList,i);
    if(object->fileStatus == 1)
    {
      while(listSize(object->writeObjects) > 0)
      {
        // TODO: optomize this method.
        // When writing lots of small blocks of data, they should be combined into a single 
        // larger file write.
        struct node *obj = popData(object->writeObjects);
        struct bufferData *data = obj->data;
        destroyList(obj);

        {
          int i;
          FILE *stream = fopen("tmp.dat","a");
          for(i=0;i<data->length;i++)
            fprintf(stream,"%i\n",data->data[i]);
          fclose(stream);
        }

        if(object->wav == NULL)
          fwrite(data->data,sizeof(short),data->length,object->stream);
        else
          WAV_WriteData(object->wav,object->stream,data);
      }
    }
  }
}

static void flushOutstandingDataInObjectToFile(struct FileWriteObject *object)
{
  if(listSize(object->writeObjects) > 0)
  {
    printf("Outstanding data: %i\n",listSize(object->writeObjects));
    writeFiles();
  }
}
static void destroyWriteObject(struct FileWriteObject *object)
{
  if(object != NULL)
  {
    free(object->fileName);
    object->fileName = NULL;

    destroyList(object->writeObjects);
    object->writeObjects = NULL;

    free(object);
    object = NULL;
  }
}


static void closePendingFiles()
{
  if(listSize(fileCloseList) > 0)
  {
    int size = 0;

    do  
    {
      struct FileWriteObject *object;
      struct node *listObject;

      pthread_mutex_lock(&writeMutex);
      size = listSize(fileCloseList) ;
      if(size > 0)
        listObject = popData(fileCloseList);
      else  
        // The fileCloseList is empty, nothing to do.
        break;
      pthread_mutex_unlock(&writeMutex);


      object = (struct FileWriteObject*)listObject->data;
      destroyList(listObject);
      listObject = NULL;

      // Remove the object from the writing list. 
      popElement(fileWriteList,object);


      flushOutstandingDataInObjectToFile(object);
      if(object->wav == NULL)
        fclose(object->stream);
      else
        closeWavSink(object->wav,object->stream);
      destroyWriteObject(object);

      size --;
    }while(size > 0);
  }
}

static volatile char fileWriteThreadInitialized = 0;

static void* fileWritingThread(void *arg)
{
  fileWriteThreadInitialized = 1;
 
  for(;;)
  {
    openPendingFiles();
    writeFiles();
    closePendingFiles();
    usleep(1000);
  }

  return NULL;
}


static struct FileWriteObject* createNewWriteObject(char *fileName)
{
  struct FileWriteObject *object = NULL;
  if(fileName != NULL)
  {
    object = (struct FileWriteObject*)malloc(sizeof(struct FileWriteObject ));
    object->writeObjects = NULL;
    object->stream = NULL;
    object->fileName = NULL;

    object->wav = NULL;

    object->fileName = (char*)malloc(sizeof(char)*128);
    memset(object->fileName,0,128);
    unsigned length = strlen(fileName);


    if(length >=128)
      strncpy(object->fileName,fileName,127);
    else
      strcpy(object->fileName,fileName);
    object->writeObjects = createNewList();

    int channels = 1;
    int codec = 0;
    object->wav = createWavSink(channels,codec);
  }

  return object;

}

static void pushHandleOntoOpenList(FileWriteHandle handle)
{
  pthread_mutex_lock(&writeMutex);
  pushData(fileOpenList,(void*)handle);
  pthread_mutex_unlock(&writeMutex);
}


// 
// Public functions.
//
FileWriteHandle getNewHandle(char *fileName)
{
  FileWriteHandle handle = createNewWriteObject(fileName);
  pushHandleOntoOpenList(handle);

  return handle;
}

void initialiseFileWriting()
{
  int ret;
  pthread_t thread;
  if(fileWriteThreadInitialized == 0)
  {
    fileOpenList  = createNewList();
    fileWriteList = createNewList();
    fileCloseList = createNewList();

    ret = pthread_create(&thread,NULL,fileWritingThread,NULL);
    if(ret == EAGAIN)
    {
      printf("ERROR: EAGAIN\n");
    }
    else if (ret == EINVAL)
    {
      printf("ERROR: EINVAL\n");
    }
    else if ( ret == EPERM)
    {
      printf("ERROR: EPERM\n");
    }

    // Wait until the writing thread is initialized.
    while(fileWriteThreadInitialized == 0)
    {
      usleep(10);
    }
  }

}

void closeFileWriteHandle(FileWriteHandle handle)
{
  pthread_mutex_lock(&writeMutex);
  pushData(fileCloseList,(void*)handle);
  pthread_mutex_unlock(&writeMutex);
}

/**
  Does a deep copy of 'data'.
 */
int writeDataChunk(FileWriteHandle handle,struct bufferData *data)
{
  if (handle != NULL && data != NULL)
  {
    struct FileWriteObject *object = (struct FileWriteObject*)handle;

    // A valid file, but it has not been opened yet.
    struct bufferData *dataCopy   = deepCopyBufferData(data); 

    pthread_mutex_lock(&writeMutex);
    pushData(object->writeObjects,dataCopy);
    pthread_mutex_unlock(&writeMutex);

    if(object->fileStatus == 1)
    {
      // Normal data, nothing to do.
    }
    else if(object->fileStatus == 0)
    {
      printf("File not opened yet; buffering data.\n");
    }
    else 
    {
      // TODO: Handle ERROR 
      printf("File Error.\n");
    }

  }
  return 0;
}

#ifdef WRITE_UNIT_TEST

int main()
{
  printf("Begin\n");
  struct bufferData data;
  short *array = (short*) malloc(sizeof(short)*160);
  data.data = array;
  data.length = 160;


  initialiseFileWriting();
  FileWriteHandle handle;

  handle = getNewHandle("test.wav");
  
  writeDataChunk(handle,&data);
  writeDataChunk(handle,&data);
  writeDataChunk(handle,&data);

  sleep(1);
  closeFileWriteHandle(handle);

  sleep(1);
  printf("End\n");

  return 0;
}
#endif
