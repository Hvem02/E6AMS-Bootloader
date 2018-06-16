// Button | Button Num | Interrupt | Port
//--------+------------+-----------+------
// UP     | 0          | INT2      | PD2
// RIGHT  | 1          | INT0      | PD0
// DOWN   | 2          | INT1      | PD1
// LEFT   | 3          | INT3      | PD3

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint-gcc.h>

#include "buttonInterface.h"

//***************************************************************
// Macros                                                       *
//***************************************************************
#define EVENT_COUNT         2
#define INTERRUPT_COUNT     4

//***************************************************************
// Static Variable Declaration                                  *
//***************************************************************
static buttonCallback_t interruptCallback[INTERRUPT_COUNT][EVENT_COUNT] = {{NULL}};
static bool buttonIsPressed[BUTTON_COUNT] = {false};

static const uint8_t buttonToInterrupt[BUTTON_COUNT] = {2, 0, 1, 3};
static const uint8_t interruptToButton[BUTTON_COUNT] = {1, 2, 0, 3};

//***************************************************************
// Static Function Declaration                                  *
//***************************************************************
void enableExternalInterrupt(uint8_t intNum);
void disableExternalInterrupt(uint8_t intNum);
bool isExternalInterruptEnabled(uint8_t intNum);
bool isInterruptUsed(uint8_t intNum);
void buttonInterruptHandler(uint8_t interruptNum);

//***************************************************************
// Public Function Implementation                               *
//***************************************************************

void buttonInit(void)
{
    DDRD &= 0b11110000;     // Port D [0-3] as input.
    EICRA |= 0b01010101;    // Interrupt on falling and rising edge.
    EIMSK |= 0b00001111;    // External interrupt enable.
    sei();
}

void buttonSetCallback(button_t button, event_t event, buttonCallback_t callbackFunc)
{
    uint8_t interruptNum = buttonToInterrupt[(uint8_t)button];
    uint8_t eventNum = (uint8_t)event;
    interruptCallback[interruptNum][eventNum] = callbackFunc;
}

//***************************************************************
// Static Function Implementation                               *
//***************************************************************
void buttonInterruptHandler(uint8_t interruptNum)
{
    uint8_t buttonNum = interruptToButton[interruptNum];
    if((PIND & (0b00000001u << interruptNum)) != (0b00000001u << interruptNum))
    {
        // Pressed
        if(interruptCallback[interruptNum][(uint8_t)PUSH] != NULL && buttonIsPressed[buttonNum] == false)
        {
            interruptCallback[interruptNum][(uint8_t)PUSH]((button_t)buttonNum, PUSH);
        }
        buttonIsPressed[buttonNum] = true;
    }
    else
    {
        // Released
        if(interruptCallback[interruptNum][(uint8_t)RELEASE] != NULL && buttonIsPressed[buttonNum] == true)
        {
            interruptCallback[interruptNum][(uint8_t)RELEASE]((button_t)buttonNum, RELEASE);
        }
        buttonIsPressed[buttonNum] = false;
    }
}

//***************************************************************
// Interrupt Vectors                                            *
//***************************************************************
#define BUTTON_ISR_FUNCTION(INT_NUM)    \
ISR(INT ## INT_NUM ## _vect)            \
{                                       \
    buttonInterruptHandler(INT_NUM);    \
}

BUTTON_ISR_FUNCTION(0);
BUTTON_ISR_FUNCTION(1);
BUTTON_ISR_FUNCTION(2);
BUTTON_ISR_FUNCTION(3);
