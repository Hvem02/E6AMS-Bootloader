//
// Created by munk on 5/26/18.
//

#ifndef E6AMS_DLLLAYER_H
#define E6AMS_DLLLAYER_H

#include <stdint.h>
#include <stdbool.h>
#include "../Library/command.h"
#include "../Frames/dllFrame.h"

uint16_t getDllSizeByCommand(Command command);
uint16_t getDllFrameSize(uint16_t appFrameSize);

void receiveDll(uint8_t uartNumber);
void initDll();
void dllSend(uint8_t* appFrame, uint16_t appFrameLength);

bool checkForFW();

#endif //E6AMS_DLLLAYER_H
