#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint-gcc.h>
#include <string.h>
#include <util/delay.h>

#include <avr/pgmspace.h>


#include "drivers/eepromFirmware.h"

#define SEGMENT_PER_PAGE 4

#define APP_AREA_ADDRESS    0x00000u
#define APP_AREA_SIZE       0x1F000u
#define TEMP_AREA_ADDRESS   0x1F000u
#define TEMP_AREA_SIZE      0x1F000u

// objcopy
// avr-objcopy -O ihex -R .eeprom E6AMS_bootloader E6AMS_bootloader.hex
// find :00000001FF

static uint8_t const * const tempPtr PROGMEM = (uint8_t*)0x1F000u;

static inline void startApp(void);

static void eepromToFlash(FirmwareFlags flags);
static void flashToFlash(FirmwareFlags flags);

static void readPage(uint32_t page, uint8_t * buffer);
static void programPage(uint32_t page, uint8_t * buffer);

static void switchPageEndianness(uint8_t* buffer);

int main()
{
    FirmwareFlags flags = readFirmwareFlags();
    if(flags.newFirmwareFlag == 0)
    {
        startApp();
    }
    else if(flags.newFirmwareFlag == 1 && flags.lastPageFlag == 0)
    {
        // Read from EEPROM and program area 2.
        eepromToFlash(flags);
        startApp();
    }
    else if(flags.newFirmwareFlag == 1 && flags.lastPageFlag == 1)
    {
        // Copy from area 2. into area 1.
        flashToFlash(flags);
        // TODO: How to read?

    }
}

static inline void startApp(void)
{
    asm volatile(
    "clr    r30 \n\t"
    "clr    r31 \n\t"
    "ijmp       \n\t");
}

static void eepromToFlash(FirmwareFlags flags)
{
    uint8_t buffer[PAGE_SIZE] = {0};
    for(uint8_t i = 0; i < flags.pageCount; i++)
    {
        readDataPage(i, buffer);
        programPage(TEMP_AREA_ADDRESS + (PAGE_SIZE * flags.pageIndex) + (PAGE_SIZE * i), buffer);
    }
    flags.pageIndex += flags.pageCount;
    writeFirmwareFlags(flags);
}

static void flashToFlash(FirmwareFlags flags)
{
    uint8_t buffer[PAGE_SIZE] = {0};
    // Copy to buffer
    memcpy(buffer, &tempPtr[0 + (flags.pageIndex * PAGE_SIZE)], PAGE_SIZE);

    switchPageEndianness(buffer);



}

static void readPage(uint32_t page, uint8_t * buffer)
{
    uint8_t sreg = SREG;
    cli();


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
        word = word + (*buffer << 8);
        buffer++;

        boot_page_fill_safe(page + i, word);
    }
    boot_page_write_safe(page);

    boot_rww_enable_safe();

    SREG = sreg;
}

static void switchPageEndianness(uint8_t* buffer)
{
    uint8_t temp = 0;
    for(uint8_t i = 0; i < SPM_PAGESIZE; i += 2)
    {
        temp = buffer[i];
        buffer[i] = buffer[i + 1];
        buffer[i + 1] = temp;
    }
}
