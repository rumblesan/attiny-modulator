#include <avr/io.h>
#include <util/delay.h>

int main (void)
{
    // Set Data Direction to output on pin 1 of PortB
    DDRB = (1<<PB0);
    while (1) {
        // set PB0 high
        PORTB = (1<<PB0);
        _delay_ms(500);
        // set PB0 low
        PORTB = (0<<PB0);
        _delay_ms(500);
    }

    return 1;
}
