#ifndef EEPROM_FIRMWARE_H
#define EEPROM_FIRMWARE_H

#include <avr/io.h>

#define PAGE_SIZE       256
#define PAGE_COUNT       16
#define FLAG_PAGE_COUNT   1
#define DATA_PAGE_COUNT  15

#define NEW_FIRMWARE_FLAG   0u
#define PAGE_COUNT_FLAG     1u
#define LAST_PAGE_FLAG      2u
#define PAGE_INDEX_FLAG_MSB 3u
#define PAGE_INDEX_FLAG_LSB 4u

#define APP_AREA_START

//***************************************************************
// Structs                                                      *
//***************************************************************

/**
 *
 */
typedef struct
{
    uint8_t newFirmwareFlag;
    uint8_t pageCount;
    uint8_t lastPageFlag;
    uint16_t pageIndex;
} FirmwareFlags;

//***************************************************************
// Public Function Definitions                                  *
//***************************************************************
/**
 *
 * @return
 */
FirmwareFlags readFirmwareFlags(void);

/**
 *
 */
void writeFirmwareFlags(FirmwareFlags flags);

/**
 *
 * @param pageNum   Between 0 and 15.
 * @param buffer    Should be 256 bytes in size.
 */
void readDataPage(uint8_t pageNum, uint8_t* buffer);

/**
 *
 * @param pageNum   Between 0 and 15.
 * @param buffer    Should be 256 bytes in size.
 */
void writeDataPage(uint8_t pageNum, uint8_t* buffer);

#endif //EEPROM_H
