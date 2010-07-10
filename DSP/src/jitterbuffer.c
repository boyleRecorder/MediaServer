
/** 
  @file jitterbuffer.c
  \brief Implementation of the jitter buffer. 

 */ 

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "jitterbuffer.h"


#include "log4c.h"

/**
  \def JITTER_BUFFER_SIZE
  \brief The size of the jitter buffer.
  */
#define JITTER_BUFFER_SIZE  0x800   // allways power of 2



/**
  \def 
  \brief We averge the size of the jitter buffer over a time duration. This sets the time.
  */
#define RUNNING_AVERAGE_COUNT 16


/*!
  \def MAX(a,b)
  \brief A macro that returns the maximum of \a a and \a b.
*/
#ifndef MAX
#define MAX(a,b) (a>b?a:b)
#endif

/*!
  \def MIN(a,b)
  \brief A macro that returns the minimum of \a a and \a b.
*/
#ifndef MIN
#define MIN(a,b) (a>b?b:a)
#endif

#ifndef USHRT_MAX_P_1
#define USHRT_MAX_P_1 (65536) // = USHRT_MAX + 1
#endif

/**
  \def JITTER_BUFFER_WRAP
  \brief Used to wrap the jitter buffer.
  */
#define JITTER_BUFFER_WRAP  (JITTER_BUFFER_SIZE-1)

/**
  \def MIN_JITTER_BUFFER_SIZE 
  \brief the minimum size in ms which the jitter buffer should use.
  */
#define MIN_JITTER_BUFFER_SIZE 40



////////////////////
// Logging stuff.
////////////////////
/*!
  \var jitterBufferLogger 
  \brief The jitter buffer logger. 

*/

log4c_category_t* jitterBufferLogger = NULL;

  
static void LOG_TRACE(const char *fmt, ...)
{
  char str[256];
  va_list arg;

  va_start(arg, fmt);
  vsprintf(str,fmt, arg);
  log4c_category_log(jitterBufferLogger,LOG4C_PRIORITY_TRACE,str);
  va_end(arg);
}

static void LOG_DEBUG(const char *fmt, ...)
{
  char str[256];
  va_list arg;

  va_start(arg, fmt);
  vsprintf(str,fmt, arg);
  log4c_category_log(jitterBufferLogger,LOG4C_PRIORITY_DEBUG,str);
  va_end(arg);
}

static void LOG_INFO(const char *fmt, ...)
{
  char str[256];
  va_list arg;
  
  va_start(arg, fmt);
  vsprintf(str,fmt, arg);
  log4c_category_log(jitterBufferLogger,LOG4C_PRIORITY_INFO,str);
  va_end(arg);
}

static void LOG_ERROR(const char *fmt, ...)
{
  char str[256];
  va_list arg;

  va_start(arg, fmt);
  vsprintf(str,fmt, arg);
  log4c_category_log(jitterBufferLogger,LOG4C_PRIORITY_ERROR,str);
  va_end(arg);
}

static void LOG_WARN(const char *fmt, ...)
{
  char str[256];
  va_list arg;

  va_start(arg, fmt);
  vsprintf(str,fmt, arg);
  log4c_category_log(jitterBufferLogger,LOG4C_PRIORITY_WARN,str);
  va_end(arg);
}

/**
  \def TRACE_BEGIN 
  \brief Trace logging. must be compiled in using the DEBUGGING flag.
  */
/**
  \def TRACE_END
  \brief Trace logging. must be compiled in using the DEBUGGING flag.
  */
#ifdef DEBUGGING
#define TRACE_BEGIN() LOG_TRACE("Begin %s(); %s:%i\n",__FUNCTION__,__FILE__,__LINE__);

#define TRACE_END()   LOG_TRACE("End   %s(); %s:%i\n",__FUNCTION__,__FILE__,__LINE__);
#else
#define TRACE_BEGIN() {}
#define TRACE_END() {}
#endif

///////////////////////////////
// Function implementations.
///////////////////////////////

JitterBufferHandle* createJitterBuffer(unsigned chanId)
{
  JitterBufferHandle *obj;
  obj = (JitterBufferHandle*)malloc(sizeof(JitterBufferHandle));

  obj->runningAverageArray = (unsigned short*)malloc(sizeof(unsigned short)*RUNNING_AVERAGE_COUNT );
  obj->jitterBufferData    = (short*)malloc(sizeof(short)*JITTER_BUFFER_SIZE);

  obj->chanId = chanId;

  return obj;
}

void destroyJitterBuffer(JitterBufferHandle *obj)
{
  free(obj->runningAverageArray);
  free(obj->jitterBufferData);
  free(obj);
}

/**
  The number of points per ms for this jitter buffer. This will 
  depend on the codec. E.g, 8 for g711, 16 for g722
  */
void setPacketSize(JitterBufferHandle *handle, unsigned short size)
{
  if(handle != NULL)
    handle->packetSize = size;
}

/**
  The estimate of the jitter is givent to the jitter buffer.
  This value should be calculate the the network polling mechanism.
  See RFC 3550 for a method of calculating the jitter. 

  The jitter buffer will attempt to maintain a dynamic size of:
  max(jitter*3, packet size * 2)

  This ought to be call periodically by the app code.

  */
void setStreamJitter(JitterBufferHandle *handle, unsigned short jitter)
{
  if(handle != NULL)
    handle->jitter = jitter;
}

/**
  Puts a block of data into the jitter buffer.
  \param handle The jitter buffer.
  \param data The data to be put into the buffer.
  */
void placeDataInBuffer(JitterBufferHandle *handle, struct bufferData *data)
{
  unsigned short i;
  TRACE_BEGIN();
  short ptr = handle->rxPtr;
  for(i=0;i<data->length;i++)
  {
    handle->jitterBufferData[ptr] = data->data[i];
    
    ptr += 1;
    ptr &= JITTER_BUFFER_WRAP  ;
  }
  TRACE_END();
}

/**
  Puts a block of data into the jitter buffer.
  \param handle The jitter buffer.
  \param data The data to be put into the buffer.
  \param offset The amount which the data is to be offset.
  */
void placeDataInBufferOffset(JitterBufferHandle *handle, struct bufferData *data, short offset)
{
  TRACE_BEGIN();
  // To advoid duplication of code, we reuse the placeDataInBuffer function.
  handle->rxPtr += offset;
  handle->rxPtr &= JITTER_BUFFER_WRAP ;

  placeDataInBuffer(handle,data);
  
  // Return to its original position.
  handle->rxPtr -= offset;
  handle->rxPtr &= JITTER_BUFFER_WRAP ;

  TRACE_END();
}


static void copyPacketInfoToJitterBuffer(JitterBufferHandle *handle, struct ReceivedPacket *packet)
{
  handle->previousSequenceNumber = packet->sequenceNumber;
  handle->previousTimeStamp      = packet->timeStamp;
}

/**
  Resets the jitter buffer to its default state.
  \param handle The jitter buffer.
  */
static void resetJitterBuffer(JitterBufferHandle *handle )
{
  handle->rxPtr = 500;
  handle->txPtr = 0;
  memset(handle->jitterBufferData,0,sizeof(handle->jitterBufferData));
  handle->averageCounter = 0;
  handle->averageBufferSize = 0;
}

/**
  Initializes the Jitter Buffer.
  \param handle The jitter buffer.
  */
void initializeBuffer(JitterBufferHandle *handle)
{
  TRACE_BEGIN();
  if(handle == NULL)
  {
    LOG_ERROR("Handle is null.");
    return;
  }

  resetJitterBuffer(handle);
  handle->firstPacket = 1;
  handle->previousSequenceNumber = 0 ;
  handle->previousTimeStamp= 0 ;
  handle->maxSeqNumber = 0;

  handle->packetSize = 8;
  handle->jitter = 0;

  TRACE_END();
}

/**
  Advances the jitter buffer, but does not add new data into it.
  \param handle The jitter buffer.
  \param advance The amount which the buffer is to be advanced.
  */
static void advanceRxJitterBuffer(JitterBufferHandle *handle, int advance)
{
  handle->rxPtr += advance;
  handle->rxPtr &= JITTER_BUFFER_WRAP ; 
}


/**
  Treats the first RTP packet in a stream.
  \param handle The jitter buffer.
  */
static void handleFirstPacket(JitterBufferHandle *handle, struct ReceivedPacket *packet)
{
  initializeBuffer(handle);
  copyPacketInfoToJitterBuffer(handle,packet);
  placeDataInBuffer(handle,packet->data);
  advanceRxJitterBuffer(handle,packet->data->length);

  handle->firstPacket = 0;
}

/**
  Returns the current size of the jitter buffer.
  \param handle The jitter buffer.
  */
static unsigned short currentJitterBufferSize(JitterBufferHandle *handle )
{
  return (handle->rxPtr - handle->txPtr) & JITTER_BUFFER_WRAP  ; 
}


/**
  The rtp sequence number is only a short. This will over-run quite often. 
  This takes into account the rollover and returns an 'int' which looks like
  a contineous value. 
  */
static unsigned int correctSequenceNumber(JitterBufferHandle *handle, unsigned short num)
{
  unsigned int ret = num;
  unsigned short shortMax = handle->maxSeqNumber & 0x0000ffff; // The 16 least significant bits.

  unsigned short mod_max = handle->maxSeqNumber >> 16; // A counter as to how many times the roll over has occured.


  if((MAX(num,shortMax)-MIN(num,shortMax)) > SHRT_MAX) // This is the discontinuity when num rolls over.
  {
    if(shortMax > num)
    {
      ret += USHRT_MAX_P_1;
    }
    else
    {
      ret -= USHRT_MAX_P_1;
    }
  }

  ret += mod_max * USHRT_MAX_P_1;



  return ret;
}

/**
  Return the time stamp difference between this packet and the previous packet.
  */
static unsigned getTimeStampDiff(JitterBufferHandle *handle, struct ReceivedPacket *packet)
{
    int timeStampDiff = packet->timeStamp - handle->previousTimeStamp;
    timeStampDiff -= packet->data->length;
    if(timeStampDiff < 0) // Something went wrong, return 0;
    {
      timeStampDiff = 0;
      LOG_ERROR("I should not have got negative ve timeStampDiff");
    }

    return (unsigned) timeStampDiff;
}

/**
  \fn handleNormalPacket(JitterBufferHandle *handle, struct ReceivedPacket *packet)
  \param handle The jitter buffer.
  \param packet An RTP packet.
  */
static void handleNormalPacket(JitterBufferHandle *handle, struct ReceivedPacket *packet)
{
  TRACE_BEGIN();

  unsigned int sequenceNumber = correctSequenceNumber(handle,packet->sequenceNumber);

  int seqDiff =  sequenceNumber - handle->previousSequenceNumber;

  if(sequenceNumber > handle->maxSeqNumber)
    handle->maxSeqNumber = sequenceNumber;

  if(seqDiff == 1)
  {
    LOG_TRACE("We have a packet in the correct order.\n");
    placeDataInBuffer(handle,packet->data);
    advanceRxJitterBuffer(handle,packet->data->length);
  }
  else if(seqDiff > 1)
  {
    LOG_INFO("We have an early packet, or packets have been dropped.\n");
    unsigned short early = packet->sequenceNumber-handle->previousSequenceNumber -1;

    unsigned advance = getTimeStampDiff(handle,packet);
    
    LOG_DEBUG("The received packet is %i out of order.\n",early);

    // Advance the jitter buffer before putting in the new data.
    advanceRxJitterBuffer(handle,advance);

    placeDataInBuffer(handle,packet->data);
    advanceRxJitterBuffer(handle,advance);

  }
  else if(seqDiff == 0)
  {
    // A duplicate packet. Do nothing.
    LOG_INFO("Got a duplicate packet.");
  }
  else  // seqDiff < 0
  {
    LOG_INFO("We have a late packet.\n");
    short offset = seqDiff*packet->data->length;
    placeDataInBufferOffset(handle,packet->data,offset);
  }

  copyPacketInfoToJitterBuffer(handle,packet);
  TRACE_END();
}

/**
  Does an average of the size of the jitter buffer over the previous time period.
  \param handle The jitter buffer.
  */
static char determineRunningAverageOfJitterBuffer(JitterBufferHandle *handle )
{
  char ret = 0;

  if(handle->averageCounter == RUNNING_AVERAGE_COUNT)
  {
    unsigned total =0;
    unsigned i;
    for (i=0;i<RUNNING_AVERAGE_COUNT;i++)
    {
      total += handle->runningAverageArray[i];
    }
    total /= RUNNING_AVERAGE_COUNT;
    ret = 1;
    handle->averageBufferSize = total;
    handle->averageCounter = 0;
  }
  else {
    unsigned short size = currentJitterBufferSize(handle);
    handle->runningAverageArray[handle->averageCounter] = size; 

    handle->averageCounter ++;
  }
  return ret;
}

/**
  Reduces the size of the jitter buffer by 1 point.
  \param handle The jitter buffer.
  */
static void reduceBufferSize(JitterBufferHandle *handle )
{
    handle->rxPtr -= 1;
    handle->rxPtr &= JITTER_BUFFER_WRAP  ;
    handle->jitterBufferData[handle->rxPtr] = 0;
}

/**
  Increases the size of the jitter buffer by 1 point.
  The new point is interpolated from the values already in the buffer.
  \param handle The jitter buffer.
  */
static void increaseBufferSize(JitterBufferHandle *handle )
{
  short ptrM1 = (handle->rxPtr -1) & JITTER_BUFFER_WRAP  ;
  short ptrM2 = (handle->rxPtr -2) & JITTER_BUFFER_WRAP  ;

  handle->rxPtr += 1;
  handle->rxPtr &= JITTER_BUFFER_WRAP  ;
  handle->jitterBufferData[handle->rxPtr] = 0;

  handle->jitterBufferData[handle->rxPtr] = (handle->jitterBufferData[ptrM1] + handle->jitterBufferData[ptrM2] ) >> 1;
}

/**
  Handles dynamic resizing of the jitter buffer.
  \param handle The jitter buffer.
  */
static void dynamicallyResizeBuffer(JitterBufferHandle *handle )
{
  if(handle->resizing == 1)
  {

    if(handle->averageBufferSize > MAX(handle->jitter,handle->packetSize*MIN_JITTER_BUFFER_SIZE))
    {
      // The buffer is too big, reduce it in size.
      LOG_TRACE("reducing the size of hte buffer: %i, %i",handle->averageBufferSize,MAX(handle->jitter,handle->packetSize*MIN_JITTER_BUFFER_SIZE));
      reduceBufferSize(handle);

      // We do not recalculate the average again until after the buffer is resized.
      handle->averageBufferSize --;
    }
    else if(handle->averageBufferSize < MIN(handle->jitter,handle->packetSize*MIN_JITTER_BUFFER_SIZE))
    {
      // The buffer is too small, increase it in size.
      LOG_INFO("Increasing the size of hte buffer.");
      increaseBufferSize(handle);

    }
    else 
    {
      LOG_TRACE("The buffer is ok.");
      // Everything okay, stop resizing.
      handle->resizing = 0;
    }
  }
  else
  {
    handle->resizing = determineRunningAverageOfJitterBuffer(handle );
  }


}


/**
  At this point we have received an audio packet. Not DTMF event.

  \param handle The jitter buffer which the packet is being put into.
  \param packet A representation of the received RTP packet.
  */
void processReceivedPacket(JitterBufferHandle *handle, struct ReceivedPacket *packet)
{
  TRACE_BEGIN();
  if(handle == NULL)
  {
    LOG_ERROR("Handle is null.");
    return;
  }

  // Handle marker bit.
  if(packet->markerBit == 1)
  {
    LOG_INFO("Got markerBit, treat like first packet.");
    handle->firstPacket = 1;
  }

  if(handle->firstPacket == 1)
  {
    LOG_INFO("Handling first packet.");
    handleFirstPacket(handle,packet);
  }
  else 
  {
    unsigned short jitterBufferSize = currentJitterBufferSize(handle);

    // If the jitter buffer is too big, or the pointers cross because
    // it has emptied, then reset it. 
    if( jitterBufferSize > (JITTER_BUFFER_SIZE  >> 1))
    {
      LOG_INFO("Resetting jitter buffer: %u, %i\n",jitterBufferSize, JITTER_BUFFER_SIZE  >> 1);
      handleFirstPacket(handle,packet);
    }
    else
    {  // We should now have proper data.
      dynamicallyResizeBuffer(handle);
      handleNormalPacket(handle,packet);
    }
  }

  TRACE_END();
}

/**
  Extracts a block of data from the jitter buffer.
  The amount of data that is extracted is determined by the 'data' parameter.
  \param handle The jitter buffer.
  \param data The object into which the data is put. 
  */
void extractData(JitterBufferHandle *handle, struct bufferData *data)
{
  unsigned int i;

  TRACE_BEGIN();

  if(handle == NULL)
  {
    LOG_ERROR("Handle is null.");
    TRACE_END();
    return;
  }

  if(currentJitterBufferSize(handle) < data->length)
  {
    LOG_INFO("Jitter Buffer is empty; resetting.\n");
    // Reset the jitter buffer.
    resetJitterBuffer(handle);
    memset(data->data,0,sizeof(short)*data->length);

    TRACE_END();
    return;
  }

  for(i=0;i<data->length;i++)
  {
    unsigned short ptr = handle->txPtr;
    data->data[i] = handle->jitterBufferData[ptr];
    handle->txPtr += 1;
    handle->txPtr &= JITTER_BUFFER_WRAP;
  }
  TRACE_END();
}

void setupJitterBufferLogger(char *logger)
{
  log4c_init();
  if(logger == NULL)
    jitterBufferLogger = log4c_category_get("prototype.log.app.jitterbuffer");
  else {
    jitterBufferLogger = log4c_category_get(logger);

  }
}

