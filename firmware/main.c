#include <avr/io.h>
#include <util/delay.h>

int main (void)
{
    // Set Data Direction to output on all port B pins
    DDRB = 0b00111111;
    while (1) {
        // set PB0 high
        PORTB = 0b00000001;
        _delay_ms(500);
        // set PB0 low
        PORTB = 0b00000000;
        _delay_ms(500);
    }

    return 1;
}
