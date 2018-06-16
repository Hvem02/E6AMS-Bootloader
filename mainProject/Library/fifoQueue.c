#include "fifoQueue.h"

//***************************************************************
// Macro Functions                                              *
//***************************************************************
#define RETURN_ON_ERROR(EXPRESSION) \
do                                  \
{                                   \
    uint8_t retVal = (EXPRESSION);  \
    if(retVal != FIFO_SUCCES)       \
    {                               \
        return retVal;              \
    }                               \
}while(0)

//***************************************************************
// Static Function Declaration                                  *
//***************************************************************
static uint8_t isFull(FifoQueue * queue);
static uint8_t isEmpty(FifoQueue * queue);

static uint16_t head(FifoQueue * queue);
static uint16_t tail(FifoQueue * queue);

//***************************************************************
// Public Function Implementation                               *
//***************************************************************
FifoQueue fifoInit(uint8_t* buffer, uint16_t size)
{
    FifoQueue fifo =
    {
        .buffer = buffer,
        .bufferSize = size,
        .nextIndex = 0,
        .usedSize = 0,
    };
    return fifo;
}

uint8_t push(FifoQueue* queue, uint8_t value)
{
    RETURN_ON_ERROR(isFull(queue));
    queue->buffer[queue->nextIndex] = value;
    queue->nextIndex = (queue->nextIndex + 1) % queue->bufferSize;
    queue->usedSize++;
    return FIFO_SUCCES;
}

uint8_t pop(FifoQueue* queue, uint8_t* retValue)
{
    RETURN_ON_ERROR(isEmpty(queue));
    *retValue = queue->buffer[tail(queue)];
    queue->usedSize--;
    return FIFO_SUCCES;
}

uint8_t clear(FifoQueue* queue)
{
    queue->nextIndex = 0;
    queue->usedSize = 0;
    return FIFO_SUCCES;
}

uint8_t fill(FifoQueue* queue, uint8_t fillValue)
{
    RETURN_ON_ERROR(clear(queue));
    for(uint16_t i = 0; i < queue->bufferSize; i++)
    {
        RETURN_ON_ERROR(push(queue, fillValue));
    }
    return FIFO_SUCCES;
}

uint8_t peak(FifoQueue * queue, uint8_t* retValue)
{
    return peakAt(queue, retValue, 0);
}

uint8_t peakAt(FifoQueue * queue, uint8_t* retValue, uint16_t peakIndex)
{
    RETURN_ON_ERROR(isEmpty(queue));
    if(peakIndex >= queue->usedSize)
    {
        return FIFO_ERROR_OUT_OF_BOUNDS;
    }

    uint16_t bufferIndex = (tail(queue) + peakIndex) % queue->bufferSize;
    *retValue = queue->buffer[bufferIndex];
    return FIFO_SUCCES;
}

//***************************************************************
// Static Function Implementation                               *
//***************************************************************
static uint8_t isFull(FifoQueue * queue)
{
    if(queue->usedSize == queue->bufferSize)
    {
        return FIFO_ERROR_FULL;
    }
    else
    {
        return FIFO_SUCCES;
    }
}

static uint8_t isEmpty(FifoQueue * queue)
{
    if(queue->usedSize == 0)
    {
        return FIFO_ERROR_EMPTY;
    }
    else
    {
        return FIFO_SUCCES;
    }
}

static uint16_t head(FifoQueue * queue)
{
    return (queue->nextIndex + queue->usedSize) % queue->bufferSize;
}

static uint16_t tail(FifoQueue * queue)
{
    return ((queue->bufferSize - queue->usedSize) + queue->nextIndex) % queue->bufferSize;
}