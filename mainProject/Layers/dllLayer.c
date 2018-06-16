//
// Created by munk on 5/26/18.
//

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "dllLayer.h"
#include "appLayer.h"

#include "../Frames/dllFrame.h"

#include "../drivers/uart.h"
#include "../drivers/hm-10.h"
#include "../drivers/buttonInterface.h"

#define BUFFER_SIZE 257

static uint8_t writeIndex = 0;
static uint8_t readIndex = 0;
static uint8_t preambleIndex = 0;

//static const uint8_t DLLMAXFRAMESIZE = 1 + 2 + 1 + 1 + 2 + 64 + 16;

enum firmwareStates {
    missing_preamble,
    more_data
};

enum firmwareStates firmwareState = missing_preamble;

// Based of the max size of a frame:
//  which is 64 bytes of payload, 16 bytes crc, 2*2 bytes of lengths, 1 byte preamble, version and cmd = 87
//  and the size of a "byte", which is via uart, hence 1 byte start, 8 databits, parity and 1 stop = 11
// at a BAUD of 115200, this is: 87*11*1/115200 = 0.0082s

ISR(TIMER0_OVF_vect) {
    // Do stuff
    firmwareState = more_data;
    // Disable interrupts
    TIMSK0 &= 0b11111000;
}

uint8_t receiveBuffer[BUFFER_SIZE] = {0};

void initDll() {
    // Setup timer0
    TCCR0A &= 0b11111100;
    TCCR0B &= 0b11110111;
    TCCR0B |= 0b00000101; // 1024 prescaler
    TCNT0 = 128;
    TIMSK0 &= 0b11111000;

    // TIFR0 bit 0 bliver sat når der er overflow, nulstilles ved at skrive 1 til det
}


uint16_t getDllFrameSize(uint16_t appFrameSize) {
    return getTotalSizeOfDllFrame(appFrameSize);
}

uint16_t getDllSizeByCommand(Command command) {
    uint16_t appSize = appFrameSize(command);
    return getDllFrameSize(appSize);
}


void receiveDll(uint8_t uartNumber) {
    uint8_t rcv;
    uartReceiveByte(uartNumber, &rcv);
    receiveBuffer[writeIndex++] = rcv;

    if (rcv == PREAMBLE) {
        TIMSK0 |= 0b1;
        preambleIndex = (uint8_t )(writeIndex-1);
    }
}

void sendAckNack(bool ack) {
    /*uint16_t totalDllFrameSize = getDllSizeByCommand(AckNack);
    uint16_t totalAppFrameSize = appFrameSize(AckNack);
    uint8_t dllFrame[totalAppFrameSize];
    uint8_t appFrame[totalAppFrameSize];
    createAckNackAppFrameBytes(appFrame, ack);
    createDllFrame(appFrame, dllFrame, totalAppFrameSize);*/

    sendAckNackAppFrameBytes(ack);

//    send(dllFrame, totalDllFrameSize);
}


bool checkForFW() {

    if (firmwareState == missing_preamble) {
        // No new data
        return false;
    }

    uint8_t bytesReady;
    bool wrappingAround = false;

    if (writeIndex > readIndex) {
        bytesReady = writeIndex - readIndex;
    } else {
        // -2, because of an additional index for ensuring \0 in BUFFER_SIZE and an additional minus for making an index
        bytesReady = (uint8_t ) (BUFFER_SIZE - 1 - readIndex + writeIndex);
        wrappingAround = true;
    }

    // Make sure we have the Preamble and two length bytes before handling data
    if (bytesReady < 3) {
        return false;
    }

    uint16_t fwFrameLength = (uint16_t) ((receiveBuffer[preambleIndex+1] << 8u) + receiveBuffer[preambleIndex+2] + 3u);
    uint16_t fwFrameLengthOrg = fwFrameLength;

    if (fwFrameLength > bytesReady) {
        return false;
    }

    uint8_t fwFrame[fwFrameLength];
    uint8_t fwFrameIndex = 0;

    if (wrappingAround) {
        fwFrameIndex = (uint8_t )(BUFFER_SIZE - preambleIndex - 1);
        memcpy(fwFrame, &receiveBuffer[preambleIndex], fwFrameIndex);
        fwFrameLength -= fwFrameIndex;
        preambleIndex = 0;
    }

    memcpy(&fwFrame[fwFrameIndex], &receiveBuffer[preambleIndex], fwFrameLength);

    if (dllFrameValid(fwFrame) == false) {
        sendAckNack(false);
        return false;
    }

    uint16_t appFrameSize = fwFrameLengthOrg - getDllSizeWithoutApp();
    uint8_t appFrame[appFrameSize];
    uint16_t appFrameStartIndex = getAppStartIndex();

    memcpy(appFrame, &fwFrame[appFrameStartIndex], appFrameSize);

    appReceive(appFrame);
    sendAckNack(true);
    firmwareState = missing_preamble;

    return true;
}

void dllSend(uint8_t* appFrame, uint16_t appFrameLength) {
    uint16_t dllSize = getTotalSizeOfDllFrame(appFrameLength);
    uint8_t dllFrame[dllSize];
    createDllFrame(appFrame, dllFrame, appFrameLength);
    send(dllFrame, dllSize);
}