#ifndef HM_10_H
#define HM_10_H

#include <stdint.h>
#include <stdbool.h>
#include "uart.h"

/**
 * Following is expected setup before using this driver:
 *  AT+BAUD4
 *  AT+PARI1
 *  AT+NOTI1
 */

//***************************************************************
// Public Defines                                               *
//***************************************************************
#define HM_10_SUCCES    0
#define HM_10_ERROR     1

//***************************************************************
// Public Function Definitions                                  *
//***************************************************************

/**
 * Setting up the HM-10 Driver, with a receive callback for when it is setup
 *
 * @param callback  A callback being called on connect/disconnect and when the host send data
 * @return          0 on success, else see UART.h for errors than can be thrown.
 */
uint8_t hm10Init(uartCallback_t callback);



/**
 * Method for sending message over uart1, with response set in buffer.
 *
 *
 * @param message   The message to send
 * @param buffer    The buffer for the response, should large enough for the response
 * @return          True if the response is a substring of OK, false if not (hence probably FAILED as response)
 */
bool sendWithResponse(const char* message, char* buffer);

/**
 * Called for checking if we can talk with the hm10 module.
 *
 * @return true if the AT command returned OK (or a substring of), else false
 */
bool hm10Ready();

/**
 * Function for reading char from uart but will return error if no value is read.
 *
 * @param uartNum   Uart being used
 * @param retVal    Pointer to value to be returned
 * @return          UART return values
 */
uint8_t readCharWithDelay(uint8_t uartNum, uint8_t* retVal);


/**
 * Method for sending an frame via bluetooth.
 *
 * @param frame     The frame to be send.
 * @param frameLen  The length of the frame to be send.
 */
bool send(uint8_t* frame, uint16_t frameLen);


#endif // MH_10_H