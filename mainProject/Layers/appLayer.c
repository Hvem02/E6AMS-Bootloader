//
// Created by munk on 5/27/18.
//

#include <string.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/wdt.h>

#include "appLayer.h"
#include "dllLayer.h"

#include "../drivers/buttonInterface.h"
#include "../Frames/appFrame.h"

static uint8_t profile = 1;
static const uint8_t profileMin = 1;
static uint8_t profileMax = 5;
static bool profileRising = true;
static uint16_t segmentsToReceive = 0;

static FwSegmentReceiveCallback_t FwSegmentReceiveCallback = NULL;

uint16_t appFrameSize(Command command) {
    return getPayloadSizeBasedOfCommand(command);
}

void createAckNackAppFrameBytes(uint8_t *appFrame, bool ack) {
    createAppFrame(appFrame, AckNack, (uint8_t *)&ack);
}


void sendAckNackAppFrameBytes(bool ack) {
    uint16_t appFrameSize = getPayloadSizeBasedOfCommand(AckNack);
    uint8_t ackFrame[appFrameSize];
    createAckNackAppFrameBytes(ackFrame, ack);
    dllSend(ackFrame, appFrameSize);
}

void setLEDs() {
    // ones = PORTB1
    // twos = PORTL1
    // fours = PORTL5
    switch (profile) {
        case 1:
            PORTL &= 0b11011101;
            PORTB |= 0b10;
            break;
        case 2:
            PORTB &= 0b11111101;
            PORTL &= 0b01111111;
            PORTL |= 0b10;
            break;
        case 3:
            PORTL &= 0b01111111;
            PORTB |= 0b10;
            PORTL |= 0b10;
            break;
        case 4:
            PORTB &= 0b11111101;
            PORTL &= 0b11111101;
            PORTL |= 0b10000000;
            break;
        case 5:
            PORTL &= 0b11011111;
            PORTB |= 0b10;
            PORTL |= 0b10000000;
            break;
        default:
            break;
    }
}

void switchProfile(button_t button, event_t event) {
    if (profileRising) {
        if (profile == profileMax) {
            profileRising = false;
            switchProfile(button, event);
            return;
        }
        ++profile;
    } else {
        if (profile == profileMin) {
            profileRising = true;
            switchProfile(button, event);
            return;
        }
        --profile;
    }
    setLEDs();
}

void sendControl(button_t button, event_t event) {
    uint16_t frameSize = appFrameSize(Control);
    uint8_t frame[frameSize];
    createControlFrame(frame, profile, button);
    dllSend(frame, frameSize);
}

void resetToBootloader() {
    // TODO Set flags that persist between reboots
    wdt_enable(WDTO_15MS);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1) {

    }
#pragma clang diagnostic pop
}

void appReceive(uint8_t* appFrame) {
    AppFrame appFrameObj;
    createAppFrameFromBytes(&appFrameObj, appFrame);
    // TODO Handle the receive

    switch (appFrameObj.cmd) {
        case FWReset:
            // Send Ack
            sendAckNackAppFrameBytes(true);
            // Set flag and reset to bootloader
            resetToBootloader();
            break;
        case FWSegCount:
            segmentsToReceive = (appFrameObj.payload[0] << 8u) + appFrameObj.payload[1];
            // Send Ack
            sendAckNackAppFrameBytes(true);
            break;
        case FWSeg:
            if(FwSegmentReceiveCallback != NULL)
            {
                FwSegmentReceiveCallback(&appFrameObj);
            }
            // Count down segmentsToReceive
            --segmentsToReceive;
            // Save the segment
            sendAckNackAppFrameBytes(true);
            break;
        case MaxProfiles:
            profileMax = appFrameObj.payload[0];
            sendAckNackAppFrameBytes(true);
            break;
        default:
            break;
    }
}

void registerFwSegmentReceiveCallback(FwSegmentReceiveCallback_t recieveCallback)
{
    FwSegmentReceiveCallback = recieveCallback;
}