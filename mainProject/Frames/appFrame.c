//
// Created by munk on 5/26/18.
//

#include <stdbool.h>
#include <string.h>
#include "appFrame.h"

const uint8_t CMD_SIZE = 1;
const uint8_t LEN_SIZE = 2;

static const uint8_t commandPayloadSize[] = {
        1, // AckNack
        2, // Control
        0, // FWReset
        0, // FWReady
        2, // FWSegmentCount
        64
};


/*uint16_t getControlAppFrameSize() {
    return CMD_SIZE+LEN_SIZE+2; // cmd + len + profile + button
}*/

uint16_t getPayloadSizeBasedOfCommand(Command command) {
    return (CMD_SIZE + LEN_SIZE + commandPayloadSize[command]);
}

void createControlFrame(uint8_t *frame, uint8_t profile, uint8_t button) {
    frame[0] = Control;
    frame[1] = 0;
    frame[2] = 2;
    frame[3] = profile;
    frame[4] = button;
}

void createAckNackAppFrame(AppFrame* appFrame, bool ack) {
    appFrame->cmd = AckNack;
    appFrame->payloadLength = 1;
    uint8_t payload = ((uint8_t) ack);
    memcpy(appFrame->payload, &payload, 1);
}

void createAppFrame(uint8_t* appFrame, Command command, uint8_t* payload) {
    appFrame[0] = command;
    uint16_t len = getPayloadSizeBasedOfCommand(command);
    appFrame[1] = (uint8_t ) ((len>>8u) & 0xFF);
    appFrame[2] = (uint8_t ) (len & 0xFF);
    memcpy(&appFrame[3], payload, len);
}

void createAppFrameFromBytes(AppFrame* appFrame, uint8_t* appFrameBytes) {
    appFrame->cmd = (Command) appFrameBytes[0];
    appFrame->payloadLength = (appFrameBytes[1] << 8u) + appFrameBytes[2];
    appFrame->payload = &appFrameBytes[3];
}