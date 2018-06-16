#ifndef FIFOQUEUE_H
#define FIFOQUEUE_H

#include <stdint-gcc.h>

//***************************************************************
// Return values                                                *
//***************************************************************
#define FIFO_SUCCES                 0x00
#define FIFO_ERROR_EMPTY            0x01
#define FIFO_ERROR_FULL             0x02
#define FIFO_ERROR_OUT_OF_BOUNDS    0x03

/**
 *
 */
typedef struct
{
    uint8_t* buffer;        /** Pointer to buffer. */
    uint16_t bufferSize;    /** Size of the buffer. */
    uint16_t nextIndex;     /** Index of next available buffer index */
    uint16_t usedSize;      /** Size of the used part of buffer. */
} FifoQueue;

/**
 *
 * @param buffer    Pointer to buffer going to be used as queue.
 * @param size      The size of the buffer.
 * @return          Returns 0 on succes.
 */
FifoQueue fifoInit(uint8_t* buffer, uint16_t size);

/**
 *
 * @param queue     Pointer to queue.
 * @param value     Value to be pushed onto queue.
 * @return          Returns 0 on succes.
 */
uint8_t push(FifoQueue* queue, uint8_t value);

/**
 *
 * @param queue     Pointer to queue.
 * @param retVal    Pointer to where value should be returned to.
 * @return          Returns 0 on succes.
 */
uint8_t pop(FifoQueue* queue, uint8_t* retValue);

/**
 *
 * @param queue     Pointer to queue.
 * @return          Returns 0 on succes.
 */
uint8_t clear(FifoQueue* queue);

/**
 *
 * @param queue     Pointer to queue.
 * @param fillValue Value to be used to fill the queue.
 * @return          Returns 0 on succes.
 */
uint8_t fill(FifoQueue* queue, uint8_t fillValue);

/**
 *
 * @param queue     Pointer to queue.
 * @param retVal    Pointer to where value should be returned to.
 * @return          Returns 0 on succes.
 */
uint8_t peak(FifoQueue * queue, uint8_t* retValue);

/**
 *
 * @param queue     Pointer to queue.
 * @param retValue  Pointer to where value should be returned to.
 * @param peakIndex Index of the value wished to be peaked.
 * @return          Returns 0 on succes.
 */
uint8_t peakAt(FifoQueue * queue, uint8_t* retValue, uint16_t peakIndex);

#endif //FIFOQUEUE_H
