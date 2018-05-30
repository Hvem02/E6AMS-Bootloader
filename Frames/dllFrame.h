//
// Created by munk on 5/26/18.
//

#ifndef E6AMS_DLLFRAME_H
#define E6AMS_DLLFRAME_H

#include <stdint.h>
#include <stdbool.h>
#include "appFrame.h"

static const uint8_t VERSION   = 0x01;
static const uint8_t PREAMBLE  = 0xAA;

typedef struct {
    uint8_t preamble;
    uint16_t length;
    uint8_t version;
    AppFrame appFrame;
    uint8_t hash[16];
} DllFrame;

/**
 * Function used for allocation of the DLL frame.
 *
 * @param appFrameSize  The size of the AppFrame
 * @return              The total size of the DLL frame including the appframe.
 */
uint16_t getTotalSizeOfDllFrame(uint16_t appFrameSize);

uint16_t getDllSizeWithoutApp();

uint16_t getAppStartIndex();

/**
 * Function for creation of a dllFrame.
 *
 * @param appFrame      The already created application layer frame (see appFrame.h)
 * @param dllFrame      The location of the resulting DLLFrame, should be allocated with function getTotalSizeOfDllFrame
 * @param appFrameSize  The size of the appframe
 */
void createDllFrame(uint8_t* appFrame, uint8_t* dllFrame, uint16_t appFrameSize);

void dllFrameFromBytes(DllFrame* res, uint8_t* frame, uint8_t* appPayload);

bool dllFrameValid(uint8_t* frame);

#endif //E6AMS_DLLFRAME_H
