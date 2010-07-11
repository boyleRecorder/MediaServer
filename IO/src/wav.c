#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "wav.h"

#define FREQUENCY 8000

/**
Writes a wav header into the array 'buffer'.
*/
// static void writeHeader(char *buffer,int numChannels,int codec);
// static void closeHeader(char *buffer, int length);


static int put_little_short(char *t, unsigned int value)
{
	*(unsigned char *)(t++)=value&255;
	*(unsigned char *)(t)=(value/256)&255;
	return 2;
}

static int put_little_long(char *t, unsigned int value)
{
	*(unsigned char *)(t++)=value&255;
	*(unsigned char *)(t++)=(value/256)&255;
	*(unsigned char *)(t++)=(value/(256*256))&255;
	*(unsigned char *)(t)=(value/(256*256*256))&255;
	return 4;
}

void writeHeader(char *buffer,int numChannels,int codec)
{
	int chunkType;
	int codecBlockSize = 1;
	int bitsPerSample = 8;
	
	switch (codec)
	{
		case PCM:
			chunkType = 1;
			codecBlockSize = 2;
			bitsPerSample = 16;
			break;
		case G711A:
			chunkType = 6;
			codecBlockSize = 1;
			bitsPerSample = 8;
			break;
		case G711U:
			chunkType = 7;
			codecBlockSize = 1;
			bitsPerSample = 8;
			break;
		default:
			break;
	}
	strcpy(buffer,"RIFF");
	/* total length will be put in later */
	strcpy(buffer+8,"WAVE"); 
	strcpy(buffer+12,"fmt ");

	put_little_long(buffer+16,16); /* I know the length  of the fmt_ chunk */
	put_little_short(buffer+20,chunkType); /* chunk type depends on codec */
	put_little_short(buffer+22,numChannels); /* two channels */
	put_little_long(buffer+24,FREQUENCY); /* samples per second */
	put_little_long(buffer+28,FREQUENCY*codecBlockSize*numChannels); /* bytes per second */
	put_little_short(buffer+32,codecBlockSize*numChannels); /* bytes pro sample (all channels) */
	put_little_short(buffer+34,bitsPerSample); /* bits per sample */

	/* data chunk, 8 bytes header and XXX bytes payload */
	strcpy(buffer+36,"data"); 

}

/**
 * \param length The length of the payload.
 */
void closeHeader(char *buffer, int length)
{
	put_little_long(buffer+40,length);
	put_little_long(buffer+4,length+8+16+8);
}

struct WavSink *createWavSink(int channels,int codec)
{
	struct WavSink *sink = (struct WavSink*)malloc(sizeof(struct WavSink));

	writeHeader(sink->header,1,codec);
  sink->length = 0;

	return sink;
}

void WAV_WriteData(struct WavSink *sink, FILE *stream, struct bufferData *data)
{
   sink->length += data->length;

   // If nothing has been written to file, write an empty 44
   // bytes at the start of the file. It will be overwritten later.
   if(ftell(stream) == 0)
   {
     char emptyHeader[WAV_HDR];
     fwrite(emptyHeader,sizeof(char),WAV_HDR,stream);
   }
   fwrite(data->data,sizeof(short),data->length,stream);
}

void closeWavSink(struct WavSink *sink, FILE *stream)
{
	closeHeader(sink->header,sink->length);

	fseek(stream,0,0);
	fwrite(sink->header,WAV_HDR,sizeof(char),stream);

	fclose(stream);
  stream = NULL;
	free(sink);
}


#ifdef DEBUG_TEST
#define SECONDS 8 /* produce 10 seconds of noise */
#define PI 3.14159265358979

/* returns the number of bytes written. skips two bytes after
 * each write */
static int fill_data(char *start, int frequency, int seconds, int channels)
{
	int i, len=0;
	int value;
	for(i=0; i<seconds*FREQUENCY; i++) {
		value=32767.0 *
			sin(2.0*PI*((double)(i))*(double)(frequency)/FREQUENCY);
		put_little_short(start, value);
		if(channels == 1)
			start += 2;
		else if(channels == 2)
			start += 4;
		len+=2;
	}
	printf("len: %i\n",len);
	return len;
}


int main(void)
{
	char *buffer=malloc(SECONDS*FREQUENCY*4+1000);
	char *t=buffer;
	int len;
	int fd;

	FileSink *sink = createWavSink("Hello.wav",1);
	writeHeader(buffer,1);

	len=fill_data(buffer+WAV_HDR,523,SECONDS); /* left channel, 450Hz sine */
	writeData(sink,buffer+WAV_HDR,len);
	//	len+=fill_data(buffer+WAV_HDR+2,452,SECONDS); /* right channel, 452Hz sine */

	closeHeader(buffer,len);
	closeWavSink(sink);

	fd=open("test.wav", O_RDWR|O_CREAT, 0644);
	write(fd,buffer,len+8+16+8+8);
	close(fd);
	return 0;
}

#endif
