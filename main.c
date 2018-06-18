#include <stdint-gcc.h>

#include <avr/io.h>
#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#include <util/delay.h>

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
static void programPage(uint32_t page, uint8_t * buffer);

static uint8_t firmwareBuffer[SPM_PAGESIZE];
static uint16_t pageIndex = 0;
static uint16_t totalSegmentCount = 0;
static uint16_t totalPageCount = 0;

int main()
{
    uartInit(0, 115200, 'O', 1, 8, 'N');
    sei();
    hm10Init(receiveDll);
    initDll();
    //registerFwSegmentCountReceivedCallback(receiveSegmentCount);
    setFWUploadHandle(receiveFwSegment);

    uint8_t fwUploadFlag = eeprom_read_byte((uint8_t *) FW_UPLOAD_FLAG);

    if (fwUploadFlag == 1)

    while(1)
    {
        _delay_ms(12);
        checkForFW();
    }

    //const uint16_t progSize = sizeof(progData);
    //const uint16_t pageSize = SPM_PAGESIZE;
    //const uint16_t lastPageWordCount = (progSize % pageSize);
    /*
    const uint16_t pageCount = 5; //(pageSize - lastPageWordCount + progSize) / pageSize;

    for(uint16_t page = 0; page < pageCount; page++)
    {
        uint8_t progBuffer[SPM_PAGESIZE];

        for(uint16_t i = 0; i < SPM_PAGESIZE; i++)
        {
            progBuffer[i] = progData[i + (page * SPM_PAGESIZE)];
        }
        programPage((page * SPM_PAGESIZE), progBuffer);
    }
    boot_rww_enable_safe();

    DDRB |= 0b11111111;
    PORTB = 0b00001111;

    for(uint8_t i = 0; i < 16; i++)
    {
        PORTB = ~PORTB;
        _delay_ms(200);
    }

    startApp();
     */
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
    uint16_t ekstraPage = ((segmentCount % SEGMENT_PR_PAGE) != 0 ? 1 : 0);
    totalPageCount = ((segmentCount - (segmentCount % SEGMENT_PR_PAGE)) / SEGMENT_PR_PAGE) + ekstraPage;
}

static void receiveFwSegment(AppFrame * appframe)
{
    for(uint8_t i = 0; i < SEMENT_SIZE; i++)
    {
        firmwareBuffer[i + (SEMENT_SIZE * (pageIndex % SEGMENT_PR_PAGE))] = appframe->payload[i];
    }

    if((pageIndex % SEGMENT_PR_PAGE) == 3)
    {
        programPage((pageIndex * SPM_PAGESIZE), firmwareBuffer);
    }

    if(pageIndex == totalPageCount)
    {
        startApp();
    }
    pageIndex++;

}

static void programPage(uint32_t page, uint8_t * buffer)
{
    uint8_t sreg = SREG;
    cli();

    boot_page_erase_safe(page);

    for(uint16_t i = 0; i < SPM_PAGESIZE; i += 2)
    {
        uint16_t word = *buffer;
        buffer++;
        word = word + (*buffer << 8u);
        buffer++;

        boot_page_fill_safe(page + i, word);
    }
    boot_page_write_safe(page);

    boot_rww_enable_safe();

    SREG = sreg;
}