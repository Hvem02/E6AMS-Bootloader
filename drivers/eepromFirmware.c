#include "eepromFirmware.h"
#include <avr/eeprom.h>

//***************************************************************
// Constants                                                    *
//***************************************************************

#define FLAG_PAGE_ADDRESS ((uint8_t*)0)
#define DATA_PAGE_ADDRESS ((uint8_t*)256)

//***************************************************************
// Public Function Implementation                               *
//***************************************************************
FirmwareFlags readFirmwareFlags(void)
{
    uint8_t buffer[PAGE_SIZE] = {0};

    eeprom_read_block((void*)&buffer, FLAG_PAGE_ADDRESS, PAGE_SIZE);

    FirmwareFlags flags =
    {
        .newFirmwareFlag    = buffer[NEW_FIRMWARE_FLAG],
        .pageCount          = buffer[PAGE_COUNT_FLAG],
        .lastPageFlag       = buffer[LAST_PAGE_FLAG],
        .pageIndex          = (uint16_t)((buffer[PAGE_INDEX_FLAG_MSB] << 8u) | buffer[PAGE_INDEX_FLAG_LSB])
    };
    return flags;
}

void writeFirmwareFlags(FirmwareFlags flags)
{
    uint8_t buffer[PAGE_SIZE]   = {0};
    buffer[NEW_FIRMWARE_FLAG]   = flags.newFirmwareFlag;
    buffer[PAGE_COUNT_FLAG]     = flags.pageCount;
    buffer[LAST_PAGE_FLAG]      = flags.lastPageFlag;
    buffer[PAGE_INDEX_FLAG_MSB] = (uint8_t)(flags.pageIndex >> 8u);
    buffer[PAGE_INDEX_FLAG_LSB] = (uint8_t)flags.pageIndex;

    eeprom_update_block((void*)&buffer, FLAG_PAGE_ADDRESS, PAGE_SIZE);
}

void readDataPage(uint8_t pageNum, uint8_t* buffer)
{
    if(pageNum <= DATA_PAGE_COUNT)
    {
        uint8_t * pageAddress = (pageNum * PAGE_SIZE) + DATA_PAGE_ADDRESS;
        eeprom_read_block((void*)buffer, pageAddress, PAGE_SIZE);
    }
}

void writeDataPage(uint8_t pageNum, uint8_t* buffer)
{
    if(pageNum <= DATA_PAGE_COUNT)
    {
        uint8_t * pageAddress = (pageNum * PAGE_SIZE) + DATA_PAGE_ADDRESS;
        eeprom_update_block((void*)buffer, pageAddress, PAGE_SIZE);
    }
}
