//
// Created by munk on 5/26/18.
//

#include <string.h>
#include <util/crc16.h>
#include "dllFrame.h"
#include "../external_libs/md5.h"
#include "../Library/command.h"
#include "appFrame.h"

static const uint8_t preambleSize = 1;
static const uint8_t lengthSize = 2;
static const uint8_t versionSize = 1;
static const uint8_t hashSize = 16;

uint16_t getTotalSizeOfDllFrame(uint16_t appFrameSize) {
    return (preambleSize + lengthSize + versionSize + appFrameSize + hashSize);
}


/**
 * This function will return the size of the dllframe counted from the version byte of the frame.
 * This is in order for getting that field and not for the total size of the dll frame.
 *
 * @param appFrameSize  The size of the AppFrame
 * @return              The size field of the DLL frame
 */
uint16_t getSizeOfDllFrame(uint16_t appFrameSize) {
    return (versionSize + appFrameSize + hashSize);
}

uint16_t getAppStartIndex() {
    return (preambleSize + lengthSize + versionSize - 1);
}


uint16_t getDllSizeWithoutApp() {
    return (preambleSize + lengthSize + versionSize + hashSize);
}

/**
 * Creating MD5 hash in the dllFrameNoMD5 input, so it should be 16B larger than dllFrameSizeNoMD5
 *
 * @param dllFrameNoMD5     The DLL frame with allocated 16 bytes at the end for MD5 checksum
 * @param dllFrameSizeNoMD5 The size of the total dll frame -16 Bytes
 */
void createHash(uint8_t* dllFrameNoMD5, uint16_t dllFrameSizeNoMD5) {
    // Following md5 usage is inspired from https://stackoverflow.com/a/8389763
    MD5_CTX md5_ctx;
    MD5Init(&md5_ctx);
    MD5Update(&md5_ctx, dllFrameNoMD5, dllFrameSizeNoMD5);
    uint8_t hash[hashSize];
    MD5Final(hash, &md5_ctx);

    memcpy(&dllFrameNoMD5[dllFrameSizeNoMD5], hash, hashSize);

}

void createDllFrame(uint8_t* appFrame, uint8_t* dllFrame, uint16_t appFrameSize) {
    dllFrame[0] = PREAMBLE;    // preamble
    uint16_t frameSize = getSizeOfDllFrame(appFrameSize); // version appFrameSize and md5 hash
    dllFrame[1] = ((uint8_t)((frameSize & 0xFF00) >> 8));
    dllFrame[2] = (uint8_t)(frameSize & 0xFF);
    dllFrame[3] = VERSION;
    memcpy(&dllFrame[4], appFrame, appFrameSize);
    createHash(dllFrame, 4+appFrameSize);
}

void dllFrameFromBytes(DllFrame* res, uint8_t* frame, uint8_t* appPayload) {
    res->preamble = frame[0];
    res->length = (frame[1] << 8u)+frame[2];
    res->version = frame[3];

    uint16_t appFramePayloadSize = ((frame[5] << 8u) + frame[6]);

    AppFrame appFrame;

    if (appFramePayloadSize > 0) {
        appFrame.cmd = (Command) frame[4];
        appFrame.payloadLength = appFramePayloadSize;
        appFrame.payload = appPayload;
        memcpy(appFrame.payload, &frame[7], appFrame.payloadLength);
    } else {
        appFrame.cmd = (Command) frame[4];
        appFrame.payloadLength = appFramePayloadSize;
    }

    res->appFrame = appFrame;
    memcpy(res->hash, &frame[7+appFrame.payloadLength], hashSize);
}

bool hashEqual(uint8_t* frame, uint16_t length) {
    uint16_t frameLengthNoMD5 = length-hashSize;
    MD5_CTX md5_ctx;
    MD5Init(&md5_ctx);
    MD5Update(&md5_ctx, frame, frameLengthNoMD5);
    uint8_t hash[hashSize];
    MD5Final(hash, &md5_ctx);
    return (memcmp(hash, &frame[frameLengthNoMD5], hashSize) == 0)? true:false;
}

bool dllFrameValid(uint8_t* frame) {

    if(frame[0] != PREAMBLE || frame[3] != VERSION) {
        return false;
    }

    uint16_t frameLength = (frame[1] << 8u) + frame[2];

    return (hashEqual(frame, frameLength));
}