
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>

#include "bufferData.h"
#include "fileread.h"

#define CHUNK_SIZE 1024

static struct node *fileOpenList;
static struct node *fileReadList;
static struct node *fileCloseList;

static pthread_mutex_t readMutex = PTHREAD_MUTEX_INITIALIZER;

static void addToReadPendingList(struct FileReadObject *object)
{
  pthread_mutex_lock(&readMutex);
  pushData(fileReadList,(void*)object);
  pthread_mutex_unlock(&readMutex);
}


static volatile char fileReadThreadInitialized = 0;


static void openPendingFiles()
{
  struct node *listObject;
  struct FileReadObject *object;
  pthread_mutex_lock(&readMutex);


    if(listSize(fileOpenList) != 0)
      printf("Size is non-zero\n");
  
  
  if(listSize(fileOpenList) > 0)
  {
    listObject = popData(fileOpenList);
    pthread_mutex_unlock(&readMutex);
    object = (struct FileReadObject*)listObject->data;
    destroyList(listObject);

    // Warning: this can block.
    FILE *stream = fopen(object->fileName,"r");
    if(stream != NULL)
    {
      object->stream = stream;

      pthread_mutex_lock(&readMutex);
      object->fileStatus = 1;
      pthread_mutex_unlock(&readMutex);
      addToReadPendingList(object);

    }
    else
    {
      object->fileStatus = -1;	
      pthread_mutex_unlock(&readMutex);
    }
  }
  else
    pthread_mutex_unlock(&readMutex);

}

static char *readWorkspace;// [CHUNK_SIZE];

static void addDataToReadList(struct node *list, struct bufferData *dataChunk)
{
  pthread_mutex_lock(&readMutex);
  pushData(list,dataChunk);
  pthread_mutex_unlock(&readMutex);
}

static void readFiles()
{
  int i;
  int size = listSize(fileReadList);
  printf("readFiles: %i\n",size);
  for(i=0;i<size;i++)
  {
    struct FileReadObject *object = getElement(fileReadList,i);
    if(object->fileStatus == 1)
    {
      size_t data_t = 0;
      printf("I have an open file\n");
      if(object->headerOpen == 0)
      {
        char header[WAV_HDR];
        printf("opening wav file header.\n");
        data_t = fread( &header,sizeof(char), WAV_HDR,object->stream);
        object->headerOpen = 1;
        object->readPosition = data_t;
      }

      // TODO: Check if stream != EOF
      
      
      printf("listSize: %i\n",listSize(object->readObjects));
      
      // Only read in 5 chunks of data at a time.
      if(listSize(object->readObjects) < 5)
      {
        // Read in a block of data, and place it into a memory chunk.
        struct bufferData *dataChunk = createBuffer(CHUNK_SIZE);
        data_t = fread(readWorkspace,sizeof(char),CHUNK_SIZE,object->stream);
        object->readPosition += data_t;
        printf("I have read: %u\n",(unsigned)data_t);

        if(data_t > 0)
        {
          addDataToReadList(object->readObjects,dataChunk);
        }

        if(data_t < CHUNK_SIZE) 
        {
          // TODO:
          // Not enough data was read in, EOF ????
          object->fileStatus = 2;
          dataChunk->length = data_t;
          addDataToReadList(object->readObjects,dataChunk);
        }
      }
      else 
      {
        printf("Buffer is already full.\n");
      }
    }
  }
}

static void* fileReadingThread(void *arg)
{
  fileReadThreadInitialized = 1;
 
  for(;;)
  {
    usleep(100000);
    printf("readingLoop\n"); 
    openPendingFiles();
    readFiles();
  }

  return NULL;
}


void initialiseFileRead()
{
  int ret;
  pthread_t thread;
  readWorkspace = (char*)malloc(sizeof(short)*CHUNK_SIZE);
  if(fileReadThreadInitialized == 0)
  {
    fileOpenList  = createNewList();
    fileReadList  = createNewList();
    fileCloseList = createNewList();

    ret = pthread_create(&thread,NULL,fileReadingThread,NULL);
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
    while(fileReadThreadInitialized == 0)
    {
      usleep(10);
    }
  }
}

static struct FileReadObject* createNewReadObject(char *fileName)
{
  struct FileReadObject *object;
  if(fileName != NULL)
  {
    object = (struct FileReadObject*)malloc(sizeof(struct FileReadObject));
    
    object->wav = NULL;
    object->readObjects = NULL;
    object->stream = NULL;
    object->headerOpen = 0;

    object->fileName = (char *)malloc(sizeof(char)*128);
  
    memset(object->fileName,0,128);
    unsigned length = strlen(fileName);


    if(length >=128)
      strncpy(object->fileName,fileName,127);
    else
      strcpy(object->fileName,fileName);


    object->readObjects = createNewList(); 

  }
  return object;
}

static void pushHandleOntoOpenList(FileReadHandle handle)
{
  pthread_mutex_lock(&readMutex);
  pushData(fileOpenList,(void*)handle);
  pthread_mutex_unlock(&readMutex);
}


FileReadHandle getNewReadHandle(char *fileName)
{
  FileReadHandle handle = createNewReadObject(fileName);
  pushHandleOntoOpenList(handle);

  return handle;
}

void closeFileReadHandle(FileReadHandle handle)
{

}

int readDataChunk(FileReadHandle handle, struct bufferData *data)
{
  return 0;
}


