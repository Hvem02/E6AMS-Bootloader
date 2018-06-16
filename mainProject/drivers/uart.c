// UDRn     USART Data Register
//          Data to be transmitted and received data is stored in this register


// UCSRnA   Control and Status Register A
//| 7     | 6     | 5     | 4     | 3     | 2     | 1     | 0     |
//| RXCn  | TXCn  | UDREn | FEn   | DORn  | UPEn  | U2Xn  | MPCMn |
//
// RXCn     : USART Receive Complete
// TXCn     : USART Transmit Complete
// UDREn    : USART Data Register Empty
// FEn      : Frame Error
// DORn     : Data OverRun
// UPEn     : USART Parity Error
// U2Xn     : Double the USART Transmission Speed
// MPCMn    : Multi-processor Communication Mode


// UCSRnB   Control and Status Register B
//| 7      | 6      | 5      | 4      | 3      | 2      | 1      | 0      |
//| RXCIEn | TXIECn | UDRIEn | RXENn  | TXENn  | UCSZn2 | RXB8n  | TXB8n  |
//
// RXCIEn   : RX Complete Interrupt Enable
// TXIECn   : TX Complete Interrupt Enable
// UDRIEn   : USART DATA Register Empty Interrupt Enable
// RXENn    : Receiver Enable
// TXENn    : Transmitter Enable
// UCSZn2   : Character Size 2
// RXB8n    : Receiver Data Bit 8
// TXB8n    : Transmitter Data Bit 8


// UCSRnC   Control and Status Register C
//| 7       | 6       | 5       | 4       | 3       | 2        | 1      | 0       |
//| UMSELn1 | UMSELn0 | UPMn1   | UPMn0   | USBSn   | UCSZn1   | UCSZn0 | UCROLn  |
//
// UMSELn1  : USART Mode Select 1
// UMSELn0  : USART Mode Select 0
// UPMn1    : Parity Mode 1
// UPMn0    : Parity Mode 0
// USBSn    : Stop Bit Select
// UCSZn1   : Character Size 1
// UCSZn0   : Character Size 0
// UCROLn   : Clock Polarity


// UBRRnH   USART Buad Rate Registers
// UBRRnL
//| 15  | 14  | 13  | 12  | 11  | 10  | 9   | 8   |
//| -   | -   | -   | -   | UBRR[11:8]            | UBRRnH
//|                         UBRR[7:0]             | UBRRnL
//| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
//
// Buadrate error in response to speed for 16 MHz
// following table is found https://arduino.stackexchange.com/a/299
// +-----------------+---------------+---------------+
// | Baud Rate (bps) | Async Normal  | Async Fast    |
// |                 | UBRRn | Error | UBRRn | Error |
// +-----------------+-------+-------+-------+-------+
// |            2400 |   416 | -0.1% |   832 |  0.0% |
// |            4800 |   207 |  0.2% |   416 | -0.1% |
// |            9600 |   103 |  0.2% |   207 |  0.2% |
// |           14400 |    68 |  0.6% |   138 | -0.1% |
// |           19200 |    51 |  0.2% |   103 |  0.2% |
// |           28800 |    34 | -0.8% |    68 |  0.6% |
// |           38400 |    25 |  0.2% |    51 |  0.2% |
// |           57600 |    16 |  2.1% |    34 | -0.8% |
// |           76800 |    12 |  0.2% |    25 |  0.2% |
// |          115200 |     8 | -3.5% |    16 |  2.1% |

#include "uart.h"
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//***************************************************************
// Macro Functions                                              *
//***************************************************************
#define OFFSET(NUM) ((NUM) == 3 ? 0x70 : 0x8 * (NUM))
#define UCSR_A(NUM) (*(&UCSR0A + OFFSET(NUM)))
#define UCSR_B(NUM) (*(&UCSR0B + OFFSET(NUM)))
#define UCSR_C(NUM) (*(&UCSR0C + OFFSET(NUM)))
#define UBRR_(NUM)  (*(&UBRR0 + OFFSET(NUM)))
#define UBRR_L(NUM) (*(&UBRR0L + OFFSET(NUM)))
#define UBRR_H(NUM) (*(&UBRR0H + OFFSET(NUM)))
#define UDR_(NUM)   (*(&UDR0 + OFFSET(NUM)))

#define RETURN_ON_ERROR(EXPRESSION) \
do                                  \
{                                   \
    uint8_t retVal = (EXPRESSION);  \
    if(retVal != UART_SUCCES)       \
    {                               \
        return retVal;              \
    }                               \
}while(0)

//***************************************************************
// Static Variable Declaration                                  *
//***************************************************************
static uartCallback_t transmitBufferEmptyCallback[4] = {NULL};
static uartCallback_t transmitByteCallback[4] = {NULL};
static uartCallback_t receiveByteCallback[4] = {NULL};

//***************************************************************
// Static Function Declaration                                  *
//***************************************************************
static uint8_t validateUartNumber(uint8_t uartNum);

static uint8_t enableTransmitterReceiver(uint8_t uartNum);
static uint8_t disableTransmitterReceiver(uint8_t uartNum);
static uint8_t enableEmptyBufferInterrupt(uint8_t uartNum);
static uint8_t disableEmptyBufferInterrupt(uint8_t uartNum);
static uint8_t enableTransmitByteInterrupt(uint8_t uartNum);
static uint8_t disableTransmitByteInterrupt(uint8_t uartNum);
static uint8_t enableReceiveByteInterrupt(uint8_t uartNum);
static uint8_t disableReceiveByteInterrupt(uint8_t uartNum);

static uint8_t getParity(uint8_t uartNum, char* retVal);
static uint8_t setParity(uint8_t uartNum, char parity);
static uint8_t getStopBits(uint8_t uartNum, uint8_t* retVal);
static uint8_t setStopBits(uint8_t uartNum, uint8_t stopBits);
static uint8_t getSynchronisationMode(uint8_t uartNum, char* syncMode);
static uint8_t setSynchronisationMode(uint8_t uartNum, char syncMode);
static uint8_t getCharacterSize(uint8_t uartNum, uint8_t* charSize);
static uint8_t setCharacterSize(uint8_t uartNum, uint8_t charSize);
static uint8_t getSpeedMode(uint8_t uartNum, char* mode);
static uint8_t setSpeedMode(uint8_t uartNum, char mode);
static uint8_t getBaudRate(uint8_t uartNum, uint32_t* baudRate);
static uint8_t setBaudRate(uint8_t uartNum, uint32_t baudRate);

static uint8_t addToTransmitBuffer(uint8_t uartNum, uint8_t byte);
static uint8_t readFromReceiveBuffer(uint8_t uartNum, uint8_t* byte);

static uint8_t checkReceiveError(uint8_t uartNum);

//***************************************************************
// Public Function Implementation                               *
//***************************************************************
uint8_t uartInit(uint8_t uartNum, uint32_t baudRate,
                 char parity, uint8_t stopBits,
                 uint8_t charSize, char mode)
{
    RETURN_ON_ERROR(validateUartNumber(uartNum));

    RETURN_ON_ERROR(enableTransmitterReceiver(uartNum));
    RETURN_ON_ERROR(setParity(uartNum, parity));
    RETURN_ON_ERROR(setStopBits(uartNum, stopBits));
    RETURN_ON_ERROR(setSynchronisationMode(uartNum, 'A'));
    RETURN_ON_ERROR(setCharacterSize(uartNum, charSize));
    RETURN_ON_ERROR(setSpeedMode(uartNum, mode));
    RETURN_ON_ERROR(setBaudRate(uartNum, baudRate));
    return UART_SUCCES;
}

uint8_t uartSetTransmitBufferEmptyCallback(uint8_t uartNum, uartCallback_t callback)
{
    RETURN_ON_ERROR(validateUartNumber(uartNum));
    if(callback != NULL)
    {
        enableEmptyBufferInterrupt(uartNum);
    }
    else
    {
        disableEmptyBufferInterrupt(uartNum);
    }
    transmitBufferEmptyCallback[uartNum] = callback;
    return UART_SUCCES;
}

uint8_t uartSetTransmitByteCallback(uint8_t uartNum, uartCallback_t callback)
{
    RETURN_ON_ERROR(validateUartNumber(uartNum));
    if(callback != NULL)
    {
        enableTransmitByteInterrupt(uartNum);
    }
    else
    {
        disableTransmitByteInterrupt(uartNum);
    }
    transmitByteCallback[uartNum] = callback;
    return UART_SUCCES;
}

uint8_t uartSetReceiveByteCallback(uint8_t uartNum, uartCallback_t callback)
{
    RETURN_ON_ERROR(validateUartNumber(uartNum));
    if(callback != NULL)
    {
        enableReceiveByteInterrupt(uartNum);
    }
    else
    {
        disableReceiveByteInterrupt(uartNum);
    }
    receiveByteCallback[uartNum] = callback;
    return UART_SUCCES;
}

uint8_t uartSendByte(uint8_t uartNum, uint8_t value)
{
    RETURN_ON_ERROR(validateUartNumber(uartNum));
    while(uartTransmitBufferEmptied(uartNum) != UART_SUCCES);
    return addToTransmitBuffer(uartNum, value);
}

uint8_t uartSendByteArray(uint8_t uartNum, uint8_t const * array, uint16_t size)
{
    RETURN_ON_ERROR(validateUartNumber(uartNum));
    for(uint16_t i = 0; i < size; i++)
    {
        while(uartTransmitBufferEmptied(uartNum) != UART_SUCCES);
        RETURN_ON_ERROR(addToTransmitBuffer(uartNum, array[i]));
    }
    return UART_SUCCES;
}

uint8_t uartSendString(uint8_t uartNum, char const * string)
{
    RETURN_ON_ERROR(validateUartNumber(uartNum));
    while(*string != '\0')
    {
        while(uartTransmitBufferEmptied(uartNum) != UART_SUCCES);
        RETURN_ON_ERROR(addToTransmitBuffer(uartNum, (uint8_t)*string));
        string++;
    }
    return UART_SUCCES;
}

uint8_t uartSendInteger(uint8_t uartNum, int16_t value, uint8_t base)
{
    RETURN_ON_ERROR(validateUartNumber(uartNum));
    uint8_t buffer[16 + 3];
    itoa(value, (char*)buffer, base);
    return uartSendString(0, (char*)buffer);
}

uint8_t uartReceiveByte(uint8_t uartNum, uint8_t* valuePtr)
{
    RETURN_ON_ERROR(validateUartNumber(uartNum));
    RETURN_ON_ERROR(uartByteReceived(uartNum));
    RETURN_ON_ERROR(checkReceiveError(uartNum));
    return readFromReceiveBuffer(uartNum, valuePtr);
}

uint8_t uartTransmitBufferEmptied(uint8_t uartNum)
{
    RETURN_ON_ERROR(validateUartNumber(uartNum));
    if((UCSR_A(uartNum) & 0b00100000u) == 0b00100000)
    {
        return UART_SUCCES;
    }
    else
    {
        return UART_BUFFER_ERROR_NOT_EMPTY;
    }
}

uint8_t uartByteTransmitted(uint8_t uartNum)
{
    RETURN_ON_ERROR(validateUartNumber(uartNum));
    if((UCSR_A(uartNum) & 0b01000000u) == 0b01000000)
    {
        return UART_SUCCES;
    }
    else
    {
        return UART_TRANSMIT_ERROR;
    }
}

uint8_t uartByteReceived(uint8_t uartNum)
{
    RETURN_ON_ERROR(validateUartNumber(uartNum));
    if((UCSR_A(uartNum) & 0b10000000u) == 0b10000000)
    {
        return UART_SUCCES;
    }
    else
    {
        return UART_RECEIVE_ERROR_NOT_RECEIVED;
    }
}

//***************************************************************
// Static Function Implementation                               *
//***************************************************************
static uint8_t validateUartNumber(uint8_t uartNum)
{
    if(uartNum <= 3)
    {
        return UART_SUCCES;
    }
    else
    {
        return UART_SETUP_ERROR_UART_NUM;
    }
}

static uint8_t enableTransmitterReceiver(uint8_t uartNum)
{
    UCSR_B(uartNum) |= 0b00011000;
    return UART_SUCCES;
}

static uint8_t disableTransmitterReceiver(uint8_t uartNum)
{
    UCSR_B(uartNum) &= 0b11100111;
    return UART_SUCCES;
}

static uint8_t enableEmptyBufferInterrupt(uint8_t uartNum)
{
    UCSR_B(uartNum) |= 0b00100000;
    return UART_SUCCES;
}

static uint8_t disableEmptyBufferInterrupt(uint8_t uartNum)
{
    UCSR_B(uartNum) &= 0b11011111;
    return UART_SUCCES;
}

static uint8_t enableTransmitByteInterrupt(uint8_t uartNum)
{
    UCSR_B(uartNum) |= 0b01000000;
    return UART_SUCCES;
}

static uint8_t disableTransmitByteInterrupt(uint8_t uartNum)
{
    UCSR_B(uartNum) &= 0b10111111;
    return UART_SUCCES;
}

static uint8_t enableReceiveByteInterrupt(uint8_t uartNum)
{
    UCSR_B(uartNum) |= 0b10000000;
    return UART_SUCCES;
}

static uint8_t disableReceiveByteInterrupt(uint8_t uartNum)
{
    UCSR_B(uartNum) &= 0b01111111;
    return UART_SUCCES;
}

static uint8_t getParity(uint8_t uartNum, char* retVal)
{
    if(((UCSR_C(uartNum) & 0b00100000u) == 0b00100000) &&
       ((UCSR_C(uartNum) | 0b11101111u) == 0b11101111))
    {
        *retVal = 'E';
    }
    else if((UCSR_C(uartNum) & 0b00110000u) == 0b00110000)
    {
        *retVal = 'O';
    }
    else if((UCSR_C(uartNum) | 0b11001111u) == 0b11001111)
    {
        *retVal = 'D';
    }
    else
    {
        return UART_SETUP_ERROR_PARITY;
    }
    return UART_SUCCES;
}

static uint8_t setParity(uint8_t uartNum, char parity)
{
    if (parity == 'E' || parity == 'e')
    {
        UCSR_C(uartNum) |= 0b00100000;   // Even Parity
        UCSR_C(uartNum) &= 0b11101111;
    }
    else if (parity == 'O' || parity == 'o')
    {
        UCSR_C(uartNum) |= 0b00110000;   // Odd Parity
    }
    else if (parity == 'D' || parity == 'd')
    {
        UCSR_C(uartNum) &= 0b11001111;   // Disabled Parity
    }
    else
    {
        return UART_SETUP_ERROR_PARITY;
    }
    return UART_SUCCES;
}

static uint8_t getStopBits(uint8_t uartNum, uint8_t* retVal)
{
    if((UCSR_C(uartNum) | 0b11110111u) == 0b11110111)
    {
        *retVal = 1;
    }
    else if((UCSR_C(uartNum) & 0b00001000u) == 0b00001000)
    {
        *retVal = 2;
    }
    else
    {
        return UART_SETUP_ERROR_STOP_BITS;
    }
    return UART_SUCCES;
}

static uint8_t setStopBits(uint8_t uartNum, uint8_t stopBits)
{
    if (stopBits == 1)
    {
        UCSR_C(uartNum) &= 0b11110111;
    }
    else if (stopBits == 2)
    {
        UCSR_C(uartNum) |= 0b00001000;
    }
    else
    {
        return UART_SETUP_ERROR_STOP_BITS;
    }
    return UART_SUCCES;
}

static uint8_t getSynchronisationMode(uint8_t uartNum, char* syncMode)
{
    if((UCSR_C(uartNum) | 0b00111111u) == 0b00111111)
    {
        *syncMode = 'A';
    }
    else if(((UCSR_C(uartNum) | 0b01111111u) == 0b01111111) &&
            ((UCSR_C(uartNum) & 0b01000000u) == 0b01000000))
    {
        *syncMode = 'S';
    }
    else
    {
        return UART_SETUP_ERROR_SYNC_MODE;
    }
    return UART_SUCCES;
}

static uint8_t setSynchronisationMode(uint8_t uartNum, char syncMode)
{
    if(syncMode == 'A' || syncMode == 'a')
    {
        UCSR_C(uartNum) &= 0b00111111;
    }
    else if(syncMode == 'S' || syncMode == 's')
    {
        UCSR_C(uartNum) &= 0b01111111;
        UCSR_C(uartNum) |= 0b01000000;
    }
    else
    {
        return UART_SETUP_ERROR_SYNC_MODE;
    }
    return UART_SUCCES;
}

static uint8_t getCharacterSize(uint8_t uartNum, uint8_t* charSize)
{
    if(((UCSR_B(uartNum) | 0b11111011u) == 0b11111011) &&
       ((UCSR_C(uartNum) | 0b11111001u) == 0b11111001))
    {
        *charSize = 5;
    }
    else if(((UCSR_B(uartNum) | 0b11111011u) == 0b11111011) &&
            ((UCSR_C(uartNum) | 0b11111011u) == 0b11111011) &&
            ((UCSR_C(uartNum) & 0b00000010u) == 0b00000010))
    {
        *charSize = 6;
    }
    else if(((UCSR_B(uartNum) | 0b11111011u) == 0b11111011) &&
            ((UCSR_C(uartNum) | 0b11111101u) == 0b11111101) &&
            ((UCSR_C(uartNum) & 0b00000100u) == 0b00000100))
    {
        *charSize = 7;
    }
    else if(((UCSR_B(uartNum) | 0b11111011u) == 0b11111011) &&
            ((UCSR_C(uartNum) & 0b00000110u) == 0b00000110))
    {
        *charSize = 8;
    }
    else
    {
        return UART_SETUP_ERROR_CHAR_SIZE;
    }
    return UART_SUCCES;
}

static uint8_t setCharacterSize(uint8_t uartNum, uint8_t charSize)
{
    if (charSize == 5)
    {
        UCSR_B(uartNum) &= 0b11111011;
        UCSR_C(uartNum) &= 0b11111001;
    }
    else if (charSize == 6)
    {
        UCSR_B(uartNum) &= 0b11111011;
        UCSR_C(uartNum) &= 0b11111011;
        UCSR_C(uartNum) |= 0b00000010;
    }
    else if (charSize == 7)
    {
        UCSR_B(uartNum) &= 0b11111011;
        UCSR_C(uartNum) &= 0b11111101;
        UCSR_C(uartNum) |= 0b00000100;
    }
    else if (charSize == 8)
    {
        UCSR_B(uartNum) &= 0b11111011;
        UCSR_C(uartNum) |= 0b00000110;
    }
    else
    {
        return UART_SETUP_ERROR_CHAR_SIZE;
    }
    return UART_SUCCES;
}

static uint8_t getSpeedMode(uint8_t uartNum, char* mode)
{
    if((UCSR_A(uartNum) | 0b11111101u) == 0b11111101)
    {
        *mode = 'N';
    }
    else if((UCSR_A(uartNum) & 0b00000010u) == 0b00000010)
    {
        *mode = 'F';
    }
    else
    {
        return UART_SETUP_ERROR_SPEED_MODE;
    }
    return UART_SUCCES;
}

static uint8_t setSpeedMode(uint8_t uartNum, char mode)
{
    if(mode == 'N' || mode == 'n')
    {
        UCSR_A(uartNum) &= 0b11111101;
    }
    else if(mode == 'F' || mode == 'f')
    {
        UCSR_A(uartNum) |= 0b00000010;
    }
    else
    {
        return UART_SETUP_ERROR_SPEED_MODE;
    }
    return UART_SUCCES;
}

static uint8_t getBaudRate(uint8_t uartNum, uint32_t* baudRate)
{
    uint16_t UBRRValue = 0;
    UBRRValue |= (uint16_t)(UBRR_H(uartNum) << 8u);
    UBRRValue |= ((uint16_t)UBRR_L(uartNum));

    char speedMode = 0;
    getSpeedMode(uartNum, &speedMode);

    if(speedMode == 'N')
    {
        *baudRate = (uint16_t)((F_CPU) / (16 * (UBRRValue + 1)));
    }
    else if(speedMode == 'F')
    {
        *baudRate = (uint16_t)((F_CPU) / (8 * (UBRRValue + 1)));
    }
    else
    {
        *baudRate = 0;
        return UART_SETUP_ERROR_SPEED_MODE;
    }
    return UART_SUCCES;
}

static uint8_t setBaudRate(uint8_t uartNum, uint32_t baudRate)
{
    if(baudRate < 100 || 115200 < baudRate)
    {
        return UART_SETUP_ERROR_BAUDRATE;
    }

    char speedMode = '\0';
    getSpeedMode(uartNum, &speedMode);
    uint16_t UBRRValue = 0;

    if(speedMode == 'N')
    {
        // Normal Speed Mode
        UBRRValue = (uint16_t)(((8 * baudRate + F_CPU) / (16 * baudRate)) - 1);
    }
    else if(speedMode == 'F')
    {
        // Fast Speed Mode
        UBRRValue = (uint16_t)(((4 * baudRate + F_CPU) / (8 * baudRate)) - 1);
    }
    else
    {
        return UART_SETUP_ERROR_BAUDRATE;
    }

    UBRR_H(uartNum) = (uint8_t)(UBRRValue >> 8u);
    UBRR_L(uartNum) = (uint8_t)(UBRRValue);

    return UART_SUCCES;
}

static uint8_t addToTransmitBuffer(uint8_t uartNum, uint8_t byte)
{
    UDR_(uartNum) = byte;
    return UART_SUCCES;
}

static uint8_t readFromReceiveBuffer(uint8_t uartNum, uint8_t* byte)
{
    *byte = UDR_(uartNum);
    return UART_SUCCES;
}

static uint8_t checkReceiveError(uint8_t uartNum)
{
    if((UCSR_A(uartNum) & 0b00010000u) == 0b00010000)
    {
       return UART_RECEIVE_ERROR_FRAME;
    }
    else if((UCSR_A(uartNum) & 0b00001000u) == 0b00001000)
    {
        return UART_RECEIVE_ERROR_DATA_OVERRUN;
    }
    else if((UCSR_A(uartNum) & 0b00000100u) == 0b00000100)
    {
        return UART_RECEIVE_ERROR_PARITY;
    }
    else
    {
        return UART_SUCCES;
    }
}

//***************************************************************
// Interrupt Vectors                                            *
//***************************************************************

// Macro function for transmit buffer full ISR
#define UART_TRANSMIT_BUFFER_ISR_FUNCTION(UART_NUM)         \
ISR(USART ## UART_NUM ## _UDRE_vect)                        \
{                                                           \
    if(transmitBufferEmptyCallback[(UART_NUM)] != NULL)     \
    {                                                       \
        transmitBufferEmptyCallback[(UART_NUM)]((UART_NUM));\
    }                                                       \
}

// Macro function for transmit byte ISR
#define UART_TRANSMIT_BYTE_ISR_FUNCTION(UART_NUM)           \
ISR(USART ## UART_NUM ## _TX_vect)                          \
{                                                           \
    if(transmitByteCallback[(UART_NUM)] != NULL)            \
    {                                                       \
        transmitByteCallback[(UART_NUM)]((UART_NUM));       \
    }                                                       \
    UCSR_A(UART_NUM) |= 0b01000000;                         \
}

// Macro function for receive byte ISR
#define UART_RECEIVE_BYTE_ISR_FUNCTION(UART_NUM)            \
ISR(USART ## UART_NUM ## _RX_vect)                          \
{                                                           \
    if(receiveByteCallback[(UART_NUM)] != NULL)             \
    {                                                       \
        receiveByteCallback[(UART_NUM)]((UART_NUM));        \
    }                                                       \
    (void)UDR_(UART_NUM);                                   \
}

// Macro function for all UART ISR's
#define UART_ISR_FUNCTIONS(UART_NUM)                        \
UART_TRANSMIT_BUFFER_ISR_FUNCTION(UART_NUM);                \
UART_TRANSMIT_BYTE_ISR_FUNCTION(UART_NUM);                  \
UART_RECEIVE_BYTE_ISR_FUNCTION(UART_NUM);

// Macro function expansions for each UART
UART_ISR_FUNCTIONS(0);
UART_ISR_FUNCTIONS(1);
UART_ISR_FUNCTIONS(2);
UART_ISR_FUNCTIONS(3);
