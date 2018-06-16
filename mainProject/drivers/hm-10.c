#include <string.h>
#include <util/delay.h>
#include "hm-10.h"

#define DEBUG_UART          0
#define HM_10_UART          3
#define HM_10_BAUDRATE      115200
#define HM_10_PARITY_BITS   1
#define HM_10_DATABITS      8
#define HM_10_START_BITS    1
#define HM_10_STOP_BITS     1

#define HM_10_ERROR_TIMEOUT 10
#define BUFFER_SIZE 257
#define SET_LEN 7

const char* atTest = "AT";

static char receiveBuffer[BUFFER_SIZE] = {0};

static uint8_t writeIndex = 0;
static uint8_t readIndex = 0;

//***************************************************************
// Macro Functions                                              *
//***************************************************************

//***************************************************************
// Static Function Declaration                                  *
//***************************************************************


//***************************************************************
// Public Function Implementation                               *
//***************************************************************


enum states {
    pre_init,
    init,
    command
};

enum states currentState;

void rcvByteCallback(uint8_t uartNumber) {
    uint8_t rcv;
    uartReceiveByte(uartNumber, &rcv);
    receiveBuffer[writeIndex++] = rcv;
}

uint8_t hm10Init(uartCallback_t callback) {
    uartSendString(DEBUG_UART, "Starting setup\n");
    currentState = pre_init;

    // Init communications
    uint8_t success = uartInit(HM_10_UART, HM_10_BAUDRATE, 'O', HM_10_PARITY_BITS, HM_10_DATABITS, 'N');

    if (success != UART_SUCCES) {
        uartSendString(DEBUG_UART, "Failed to setup uart for HM-10");
        return success;
    }

    success = uartSetReceiveByteCallback(HM_10_UART, rcvByteCallback);

    if (success != UART_SUCCES) {
        uartSendString(DEBUG_UART, "Failed to setup callback for HM-10");
        return success;
    }


    // Check that the HM-10 is ready to receive commands

    while (currentState != command) {

        // Handle read of data

        char* index = strstr(&receiveBuffer[readIndex], "OK");

        if (index != NULL) {
            // Not null, hence what we asked for is in the buffer
            switch (currentState) {
                case pre_init:
                    readIndex = (uint8_t) ((receiveBuffer-index) + 2);
                    currentState = command;
                    break;
                case init:
                    currentState = command;
                    break;
                default:
                    uartSendString(DEBUG_UART, "Default2\n");
                    break;
            }
        }

        // Handle send of strings based of the state
        switch (currentState) {
            case pre_init:
                uartSendString(HM_10_UART, atTest);
                break;
            default:
                uartSendString(DEBUG_UART, "Default\n");
                break;
        }
        _delay_ms(16);
        _delay_ms(16);
        _delay_ms(16);
    }

    success = uartSetReceiveByteCallback(HM_10_UART, callback);

    if (success != UART_SUCCES) {
        uartSendString(DEBUG_UART, "Failed to setup external callback on bytes received for hm10");
        return success;
    }

    uartSendString(DEBUG_UART, "Done setup hm10!");

    return 0;
}

bool send(uint8_t* frame, uint16_t frameLen) {
    if (hm10Ready()) {
        uartSendByteArray(HM_10_UART, frame, frameLen);
        return true;
    } else {
        uartSendString(DEBUG_UART, "Tried to send frame but HM10 not ready!");
        return false;
    }
}

bool hm10Ready() {
    return (currentState == command)? true:false;
}

//***************************************************************
// Static Function Implementation                               *
//***************************************************************
