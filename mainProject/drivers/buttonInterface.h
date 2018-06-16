#ifndef BUTTON_INTERFACE_H
#define BUTTON_INTERFACE_H

#define BUTTON_COUNT        4

//***************************************************************
// Enums                                                        *
//***************************************************************
/**
 *  Enum for buttom number.
 */
typedef enum
{
    UP    = 0,
    RIGHT = 1,
    DOWN  = 2,
    LEFT  = 3,
} button_t;

/**
 * Enum for event type number.
 */
typedef enum
{
    PUSH    = 0,
    RELEASE = 1,
} event_t;

//***************************************************************
// Function Pointer Declaration for callbacks                   *
//***************************************************************
/**
 * @param button        Enum for which button to use.
 * @param event         Enum for push or release event.
 */
typedef void (* buttonCallback_t)(button_t button, event_t event);

//***************************************************************
// Public Function Definitions                                  *
//***************************************************************
/**
 * Function for initialising the button interface.
 */
void buttonInit(void);

/**
 * Function used to register callback function for when button is pressed or released.
 * To unregister the callback function, register NULL instead.
 *
 * @param button        Enum for which button to use.
 * @param event         Enum for push or release event.
 * @param callbackFunc  Callback function to be called when button is pressed or released.
 */
void buttonSetCallback(button_t button, event_t event, buttonCallback_t callbackFunc);

#endif //BUTTON_INTERFACE_H
