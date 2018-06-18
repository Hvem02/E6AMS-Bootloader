#include <stdint-gcc.h>

#include <avr/io.h>
#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#include <util/delay.h>
#include <string.h>

#include "mainProject/Drivers/hm-10.h"
#include "mainProject/Layers/appLayer.h"
#include "mainProject/Layers/dllLayer.h"
#include "mainProject/Frames/appFrame.h"

#define SEGMENT_PR_PAGE 4
#define SEMENT_SIZE     64


// objcopy
// avr-objcopy -O ihex -R .eeprom E6AMS_bootloader E6AMS_bootloader.hex
// find :00000001FF

static inline void startApp(void);
static void receiveSegmentCount(uint16_t segmentCount);
static void receiveFwSegment(AppFrame * appframe);
static void programPage(uint32_t page);

static uint8_t firmwareBuffer[SPM_PAGESIZE] = {0};
static uint8_t firmwareBufferIndex = 0;
static uint16_t pageIndex = 0;
static uint16_t totalSegmentCount = 0;
static uint16_t totalPageCount = 0;

int main()
{
    uartInit(0, 115200, 'O', 1, 8, 'N');
    sei();
    hm10Init(receiveDll);
    initDll();
    setFWUploadHandle(receiveFwSegment);

    uint8_t fwUploadFlag = eeprom_read_byte((uint8_t *) FW_UPLOAD_FLAG);

    if (fwUploadFlag == FW_UPLOAD_READY) {
        // We are in the progress of updating the firmware
        // Reset the flag
        eeprom_update_byte((uint8_t *) FW_UPLOAD_FLAG, 0);
        // Send message to host of ready for firmware upload
        sendReadyForFWUpload();

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
        while(1)
        {
            _delay_ms(12);
            checkForFW();
        }
#pragma clang diagnostic pop
    } else {
        // No need to fw upload, start the app instead
        startApp();
    }
}

static inline void startApp(void)
{
    asm volatile(
    "clr    r30 \n\t"
    "clr    r31 \n\t"
    "ijmp       \n\t");
}

static void receiveSegmentCount(uint16_t segmentCount)
{
    totalSegmentCount = segmentCount;
    uint16_t extraPage = ((segmentCount % SEGMENT_PR_PAGE) != 0 ? 1 : 0);
    totalPageCount = ((segmentCount - (segmentCount % SEGMENT_PR_PAGE)) / SEGMENT_PR_PAGE) + extraPage;
}

static void receiveFwSegment(AppFrame* appframe)
{
    if (appframe->cmd == FWSegCount) {
        uint16_t segmentCount = (appframe->payload[0] << 8u) + appframe->payload[1];
        receiveSegmentCount(segmentCount);
        return;
    } else if (appframe->cmd == FWSeg) {
        // Copy the received data into the buffer
        memcpy(&firmwareBuffer[firmwareBufferIndex], appframe->payload, appframe->payloadLength);
        // Count up the buffer index
        firmwareBufferIndex += appframe->payloadLength;
        // Convert from size to index
        uint16_t totalPageIndex = totalPageCount-1;

        if(((pageIndex % SEGMENT_PR_PAGE) == 3) || (pageIndex == totalPageIndex))
        {
            programPage((pageIndex * SPM_PAGESIZE));
        }

        if(pageIndex == totalPageIndex)
        {
            uartSendString(0, "Starting the program\n");
            startApp();
        }

        pageIndex++;
    } else {
        uartSendString(0, "I do not handle command ");
        uartSendInteger(0, appframe->cmd, 10);
        uartSendByte(0, '\n');
    }
}

static void programPage(uint32_t page)
{
    uint8_t sreg = SREG;
    cli();

    boot_page_erase_safe(page);

    for(uint16_t i = 0; i < SPM_PAGESIZE; i += 2) {
        uint16_t word = 0;

        if (i < firmwareBufferIndex) {
            word = firmwareBuffer[i];
            if (i+1 < firmwareBufferIndex) {
                word = word + (firmwareBuffer[i+1] << 8u);
            }
        }

        boot_page_fill_safe(page + i, word);
    }

    firmwareBufferIndex = 0;

    boot_page_write_safe(page);

    boot_rww_enable_safe();

    SREG = sreg;
}