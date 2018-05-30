#ifndef E6AMS_APPLAYER_H
#define E6AMS_APPLAYER_H

#include <stdint.h>

#include "../Frames/appFrame.h"

uint16_t appFrameSize(Command command);

void createAckNackAppFrameBytes(uint8_t *appFrame, bool ack);
void sendAckNackAppFrameBytes(bool ack);
void setLEDs();
void appReceive(uint8_t* appFrame);

/**
 *
 */
typedef void (* FwSegmentCountReceivedCallback_t)(uint16_t segmentCount);

/**
 *
 */
void registerFwSegmentCountReceivedCallback(FwSegmentCountReceivedCallback_t receiveSegmentCountCallback);

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
