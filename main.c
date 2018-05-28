#include <avr/boot.h>
#include <avr/io.h>
#include <util/delay.h>

// objcopy
// avr-objcopy -O ihex -R .eeprom E6AMS_bootloader E6AMS_bootloader.hex
// find :00000001FF

inline void startApp(void)
{
    asm volatile(
    "clr    r30 \n\t"
    "clr    r31 \n\t"
    "ijmp   \n\t");
}

int main()
{
    DDRB |= 0b11111111;
    PORTB = 0b00001111;

    uint8_t blinker = 0b00001100;

    for(uint8_t i = 0; i < 16; i++)
    {
        PORTB = ((blinker << 4) | i);
        blinker = ~blinker;
        _delay_ms(200);
    }

    startApp();
}

