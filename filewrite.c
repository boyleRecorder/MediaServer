#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "filewrite.h"

static struct node *fileOpenList;
static struct node *fileWriteList;
static struct node *fileCloseList;

static pthread_mutex_t writeMutex = PTHREAD_MUTEX_INITIALIZER;

static void addToWritePendingList(struct FileWriteObject *object)
{
	pthread_mutex_lock(&writeMutex);
	pushData(fileWriteList,(void*)handle);
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
		FILE *stream = fopen(object->fileName,"a");
		if(stream != NULL)
		{
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
}

static void* fileWritingThread(void *arg)
{

	for(;;)
	{
		openPendingFiles();
		sleep(1);
	}

	return NULL;
}


static struct FileWriteObject* createNewWriteObject(char *fileName)
{
	struct FileWriteObject *object = NULL;
	if(fileName != NULL)
	{
		object = (struct FileWriteObject*)malloc(sizeof(struct FileWriteObject *));
		object->writeObjects = NULL;
		object->stream = NULL;
		object->fileName = NULL;

		object->fileName = (char*)malloc(sizeof(char)*128);
		unsigned length = strlen(fileName);
		if(length >=128)
			strncpy(object->fileName,fileName,128);
		else
			strncpy(object->fileName,fileName,length);
	}

	return object;

}

static void pushHandleOntoOpenList(handle)
{
	pthread_mutex_lock(&writeMutex);
	pushData(fileOpenList,(void*)handle);
	pthread_mutex_unlock(&writeMutex);
	/*
	pthread_mutex_lock(&writeMutex);
	struct FileWriteObject *object = (struct FileWriteObject*)handle;
	printf("pushing: %s\n",object->fileName);
	pushData(fileOpenList,(void*)handle);
	struct node *data;

	data = popData(fileOpenList);
	object = data->data;

	destroyList(data);

	printf("poping:  %s\n",object->fileName);
	exit(0);
	pthread_mutex_unlock(&writeMutex);

	*/
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
	pthread_t thread;
	fileOpenList  = createNewList();
	fileWriteList = createNewList();
	fileCloseList = createNewList();

	pthread_create(&thread,NULL,fileWritingThread,NULL);

}

void closeFileWriteHandle(FileWriteHandle handle)
{
}

int writeDataChunk(FileWriteHandle handle,struct bufferData *data)
{
	handle
}

#ifdef UNIT_TEST

int main()
{
	printf("Begin\n");
	struct bufferData data;
	initialiseFileWriting();
	FileWriteHandle handle;
	
	handle = getNewHandle("test.wav");

	writeDataChunk(handle,&data);

	closeFileWriteHandle(handle);

	sleep(3);
	printf("End\n");

	return 0;
}
#endif
