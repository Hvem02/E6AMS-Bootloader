#ifndef UART_H
#define UART_H

#include <stdint-gcc.h>
#include <stdbool.h>

//***************************************************************
// Error return values                                          *
//***************************************************************
#define UART_SUCCES                     0x00
#define UART_SETUP_ERROR_UART_NUM       0x01
#define UART_SETUP_ERROR_PARITY         0x02
#define UART_SETUP_ERROR_STOP_BITS      0x03
#define UART_SETUP_ERROR_SYNC_MODE      0x04
#define UART_SETUP_ERROR_CHAR_SIZE      0x05
#define UART_SETUP_ERROR_SPEED_MODE     0x06
#define UART_SETUP_ERROR_BAUDRATE       0x07
#define UART_BUFFER_ERROR_NOT_EMPTY     0x08
#define UART_TRANSMIT_ERROR             0x09
#define UART_RECEIVE_ERROR_NOT_RECEIVED 0x0A
#define UART_RECEIVE_ERROR_FRAME        0x0B
#define UART_RECEIVE_ERROR_DATA_OVERRUN 0x0C
#define UART_RECEIVE_ERROR_PARITY       0x0D

//***************************************************************
// Public Macro Functions                                       *
//***************************************************************
#define UART_BLOCKING(EXPRESSION) while(EXPRESSION != UART_SUCCES)

//***************************************************************
// Function Pointer Declaration for callbacks                   *
//***************************************************************
typedef void (* uartCallback_t)(uint8_t uartNum);

//***************************************************************
// Public Function Definitions                                  *
//***************************************************************
/**
 * Function for setting up UART.
 *
 * @param uartNum   Which UART is used: 0 - 3.
 * @param baudRate  Baudrate for the UART: 110 - 115200.
 * @param parity    Is parity used: 'E' for even, 'O' for odd and 'D' for disabled.
 * @param stopBits  Number of stopbits used: 1 - 2.
 * @param charSize  Size of characters: 5 - 8, 9-bit not supported.
 * @param mode      Normal or fast mode: 'N' or 'F'.
 * @return          Returns 0 on succes.
 */
uint8_t uartInit(uint8_t uartNum, uint32_t baudRate,
              char parity, uint8_t stopBits,
              uint8_t charSize, char mode);

/**
 * Function for setting callback function for when the transmit buffer is empty.
 * The interrupt can be disabled by passing NULL to this function.
 *
 * @param uartNum   Which UART is used: 0 - 3.
 * @param callback  Callback function pointer.
 * @return          Returns 0 on succes.
 */
uint8_t uartSetTransmitBufferEmptyCallback(uint8_t uartNum, uartCallback_t callback);

/**
 * Function for setting callback function for when a byte has been transmitted.
 * The interrupt can be disabled by passing NULL to this function.
 *
 * @param uartNum   Which UART is used: 0 - 3.
 * @param callback  Callback function pointer.
 * @return          Returns 0 on succes.
 */
uint8_t uartSetTransmitByteCallback(uint8_t uartNum, uartCallback_t callback);

/**
 * Function for setting callback function for when a byte has be received.
 * The interrupt can be disabled by passing NULL to this function.
 *
 * @param uartNum   Which UART is used: 0 - 3.
 * @param callback  Callback function pointer.
 * @return          Returns 0 on succes.
 */
uint8_t uartSetReceiveByteCallback(uint8_t uartNum, uartCallback_t callback);

/**
 * Function for sending a single byte.
 * This function is non-blocking and will return error if it not possible to send byte.
 *
 * @param uartNum   Which UART is used: 0 - 3.
 * @param value     Character to be sent.
 * @return          Returns 0 on succes.
 */
uint8_t uartSendByte(uint8_t uartNum, uint8_t value);

/**
 * Function for sending an array of bytes.
 * This function blocks until the array has been sent.
 *
 * @param uartNum   Which UART is used: 0 - 3.
 * @param array     Array of bytes to be sent.
 * @param size      Size of array.
 * @return          Returns 0 on succes.
 */
uint8_t uartSendByteArray(uint8_t uartNum, uint8_t const * array, uint16_t size);

/**
 * Function for sending a null-terminated string.
 * The null-terminating will also be sent.
 *
 * @param uartNum   Which UART is used: 0 - 3.
 * @param string    Null terminated string to be sendt.
 * @return          Returns 0 on succes.
 */
uint8_t uartSendString(uint8_t uartNum, char const * string);

/**
 * Function for sending an integer formated as text.
 *
 * @param uartNum   Which UART is used: 0 - 3.
 * @param value     Value to be sent.
 * @param base      Base value has to written in: 2 - 36.
 * @return          Returns 0 on succes.
 */
uint8_t uartSendInteger(uint8_t uartNum, int16_t value, uint8_t base);

/**
 * Function for receiving a single byte.
 * This function is non-blocking and will return an error if no byte has been received.
 *
 * @param uartNum   Which UART is used: 0 - 3.
 * @param value     Pointer to variable to hold received character.
 * @return          Returns 0 on succes.
 */
uint8_t uartReceiveByte(uint8_t uartNum, uint8_t* valuePtr);

/**
 * Function for checking if the transmitbuffer is empty.
 *
 * @param uartNum   Which UART is used: 0 - 3.
 * @return          Returns 0 on if transmit buffer is empty.
 */
uint8_t uartTransmitBufferEmptied(uint8_t uartNum);

/**
 * Function for checking if a byte has been transmitted.
 *
 * @param uartNum   Which UART is used: 0 - 3.
 * @return          Returns 0 on if byte is transmitted.
 */
uint8_t uartByteTransmitted(uint8_t uartNum);

/**
 * Function for checking if a byte has been received.
 *
 * @param uartNum   Which UART is used: 0 - 3.
 * @return          Returns 0 on if byte is received.
 */
uint8_t uartByteReceived(uint8_t uartNum);

#endif //UART_H
