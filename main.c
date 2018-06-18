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

// objcopy
// avr-objcopy -O ihex -R .eeprom E6AMS_bootloader E6AMS_bootloader.hex
// find :00000001FF

static inline void startApp(void);
static void receiveFwSegment(AppFrame * appframe);
static void programPage(uint32_t page);

static uint8_t firmwareBuffer[SPM_PAGESIZE] = {0};
static uint8_t firmwareBufferIndex = 0;
static uint16_t pageIndex = 0;
static uint16_t expectedSegmentsToRcv = 0;
static uint16_t totalSegmentRcv = 0;

int main()
{
    uartInit(0, 115200, 'O', 1, 8, 'N');
    sei();
    uartSendString(0, "Starting bootloader\n");
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


static void receiveFwSegment(AppFrame* appframe) {

    if (appframe->cmd == FWSegCount) {
        expectedSegmentsToRcv = (appframe->payload[0] << 8u) + appframe->payload[1];
        uartSendString(0, "Getting segments: ");
        uartSendInteger(0, expectedSegmentsToRcv, 10);
        uartSendString(0, "\n");
    } else if (appframe->cmd == FWSeg) {
        ++totalSegmentRcv;
        // Copy the received data into the buffer
        memcpy(&firmwareBuffer[firmwareBufferIndex], appframe->payload, appframe->payloadLength);
        // Count up the buffer index
        firmwareBufferIndex += appframe->payloadLength;

        if ((totalSegmentRcv % 4) == 0) {
            // We've got too much data, write the page
            programPage((pageIndex++ * SPM_PAGESIZE));
            firmwareBufferIndex = 0;

            if (totalSegmentRcv == expectedSegmentsToRcv) {
                uartSendString(0, "Starting the program\n");
                startApp();
            }
        }

        if (totalSegmentRcv == expectedSegmentsToRcv) {
            // We've got too much data, write the page
            programPage((pageIndex++ * SPM_PAGESIZE));
            firmwareBufferIndex = 0;
            uartSendString(0, "Starting the program\n");
            startApp();
        }

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
        // Word in little endian!
        uint16_t word = 0;

        if (i < firmwareBufferIndex) {
            word = firmwareBuffer[i];
            if (i + 1 < firmwareBufferIndex) {
                word += (firmwareBuffer[i + 1] << 8u);
            }
        }

        boot_page_fill_safe(page + i, word);
    }

    firmwareBufferIndex = 0;

    boot_page_write_safe(page);

    boot_rww_enable_safe();

    SREG = sreg;
}

char cSREG;
cSREG = SREG; /* store SREG value */
/* disable interrupts during timed sequence */
__disable_interrupt();
EECR |= (1<<EEMPE); /* start EEPROM write */
EECR |= (1<<EEPE);
SREG = cSREG; /* restore SREG value (I-bit) */