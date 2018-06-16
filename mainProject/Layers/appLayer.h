//
// Created by munk on 5/27/18.
//

#ifndef E6AMS_APPLAYER_H
#define E6AMS_APPLAYER_H

#include <stdint.h>

#include "../Frames/appFrame.h"
#include "../drivers/buttonInterface.h"

uint16_t appFrameSize(Command command);

void createAckNackAppFrameBytes(uint8_t *appFrame, bool ack);
void sendAckNackAppFrameBytes(bool ack);
void sendControl(button_t button, event_t event);
void switchProfile(button_t button, event_t event);
void setLEDs();
void appReceive(uint8_t* appFrame);

/**
 *
 */
typedef void (* FwSegmentReceiveCallback_t)(AppFrame * appframe);

/**
 *
 * @param recieveCallback
 */
void registerFwSegmentReceiveCallback(FwSegmentReceiveCallback_t recieveCallback);

#endif //E6AMS_APPLAYER_H
