//
// Created by munk on 3/15/18.
//

/*
 * Buadrate error in response to speed for 16 MHz
 * following table is found https://arduino.stackexchange.com/a/299
 *
 * U2Xn = 0 => Async Normal mode
 * U2Xn = 1 => Async double speed mode
 *
 * +--------------------+---------------+---------------+
 * | Baud Rate (bps)    | U2Xn = 0      | U2Xn = 1      |
 * |                    | UBRRn | Error | UBRRn | Error |
 * +--------------------+-------+-------+-------+-------+
 * | 2400               | 416   | -0.1% | 832   | 0.0%  |
 * | 4800               | 207   | 0.2%  | 416   | -0.1% |
 * | 9600               | 103   | 0.2%  | 207   | 0.2%  |
 * | 14400              | 68    | 0.6%  | 138   | -0.1% |
 * | 19200              | 51    | 0.2%  | 103   | 0.2%  |
 * | 28800              | 34    | -0.8% | 68    | 0.6%  |
 * | 38400              | 25    | 0.2%  | 51    | 0.2%  |
 * | 57600              | 16    | 2.1%  | 34    | -0.8% |
 * | 76800              | 12    | 0.2%  | 25    | 0.2%  |
 * | 115200             | 8     | -3.5% | 16    | 2.1%  |
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#include "drivers/uart.h"
#include "drivers/hm-10.h"
#include "drivers/buttonInterface.h"

#include "Layers/dllLayer.h"
#include "Layers/appLayer.h"

#include "external_libs/md5.h"

ISR(__vector_default){};

void mainProgram(void);
void testProgramAlex(void);
void testProgramSoren(void);

int main()
{
//    mainProgram();
    testProgramAlex();
//    testProgramSoren();
}

void buttonLeftCallback(void) {

}

void mainProgram(void)
{
    uartInit(0, 115200, 'O', 1, 8, 'N');
    sei();
    hm10Init(receiveDll);
//    buttonInterfaceInit();
//    buttonSetLeftPushCallback(buttonLeftCallback);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(1) {

    }
#pragma clang diagnostic pop
}

void initLEDs() {
    DDRB |= 0b10;   // PB1
    DDRL |= 0b10000010;   // PL7+PL1
}

void testProgramAlex(void)
{
    uartInit(0, 115200, 'O', 1, 8, 'N');
    sei();
    hm10Init(receiveDll);
    buttonInit();
    buttonSetCallback(LEFT, PUSH, sendControl);
    buttonSetCallback(RIGHT, PUSH, sendControl);
    buttonSetCallback(UP, PUSH, switchProfile);
    initLEDs();
    setLEDs();
    initDll();

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(1)
    {
        _delay_ms(12);
        checkForFW();
    }
#pragma clang diagnostic pop
}

uint8_t buffer[100] = {0};
uint8_t counter = 0;

void uartHandler(uint8_t uartNum)
{
    uartSendString(0, "Got the following: ");
    uint8_t ch = 0;
    uartReceiveByte(uartNum, &ch);
    uartSendByte(uartNum, ch);
    uartSendString(0, "Test: ");
    uartSendByte(0, '0');
}

void testProgramSoren(void)
{
    uartInit(0, 115200, 'E', 1, 8, 'N');
    //uartInit(1, 9600, 'D', 1, 8, 'N');

    uartSendString(0, "Test\n\r");

    sei();
    uartSetReceiveByteCallback(0, uartHandler);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(1)
    {
    }
#pragma clang diagnostic pop
}