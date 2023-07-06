#include <avr/io.h>
#include <util/delay.h>

#include "../lib/edgedetector.h"

#define LED_PIN PB0
#define TRIG_PIN PB1

#define LOW 0
#define HIGH 1

bool led_state = LOW;

int main (void)
{
    EdgeDetector ed;
    edge_detector_init(ed);

    // LED output
    // TRIG input
    DDRB |= (1<<LED_PIN);
    DDRB &= ~(1<<TRIG_PIN);

    // set pullup resistor for trig pin
    PORTB |= (1<<TRIG_PIN);

    while (1) {
        edge_detector_update(ed, (PINB & (1<<TRIG_PIN))>0);
        if (edge_detector_is_rising(ed)) {
            led_state = HIGH;
        } else if (edge_detector_is_falling(ed)) {
            led_state = LOW;
        }

        if (led_state == HIGH) {
            PORTB |= (1<<LED_PIN);
        } else {
            PORTB &= ~(1<<LED_PIN);
        }

        _delay_ms(500);

    }

    return 1;
}
