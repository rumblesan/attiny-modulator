#include <avr/io.h>

#define F_CPU 8000000

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "oscillator.h"
#include "ticker.h"
#include "wavetables.h"

#define MAX_ADC_VALUE 1023

#define PARAM_1_IN_MUX 0b00001
#define PARAM_2_IN_MUX 0b00010
#define PARAM_3_IN_MUX 0b00011

#define TRIG_IN_PIN PB0
#define PWM_OUT_PIN PB1

#define LOW 0
#define HIGH 1

volatile uint8_t adc_read_channel = PARAM_1_IN_MUX;

uint16_t volatile param_1_adc_in;
uint16_t volatile param_2_adc_in;
uint16_t volatile param_3_adc_in;

volatile Oscillator lfo;

const uint8_t *lfo_wavetable;

void startADCConversion(uint8_t adc_channel) {
  ADMUX  = adc_channel;
  ADCSRA |= _BV(ADSC);
}

void setupPWM() {

  // Enable PLL
  PLLCSR |= _BV(PLLE);

  // Datasheet recommends waiting 100us to avoid initial noise in the PLL
  // getting locked
  _delay_us(100);

  // Wait for PLL to be locked (denoted by PLOCK bit in PLLCSR)
  loop_until_bit_is_set(PLLCSR, PLOCK);

  // Set PCKE to use PLL as the clock for Timer/Counter1
  PLLCSR |= _BV(PCKE);

  // Timer/Counter1 control status register TCCR1:
  // PWM1A - Enable PWM generator A for Timer/Counter1
  TCCR1 |= _BV(PWM1A);
  // Set no prescaler so PWM runs at 250 kHz
  TCCR1 |= _BV(CS10);

  // Enable PWM output;
  TCCR1 |= _BV(COM1A1);

  DDRB |= _BV(PWM_OUT_PIN);

}

void setupADCInputs() {
  ADMUX = 0;
                                     // ADC clock runs at main clock / 128
  ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
  ADCSRA |= _BV(ADIE);               // Enable the conversion finish interrupt
  ADCSRA |= _BV(ADEN);               // Enable the ADC
}

void setupTimer() {

  /* Timer 0 Setup */
  // Timer0 gets setup to call the TIM_OVF interrupt at a 20kHz frequency

  TCCR0A |= _BV(WGM01) | _BV(WGM00);  // Enable Fast PWM mode
  TCCR0B |= _BV(WGM02);               // Overflow when the counter reaches TOP
  TCCR0B |= _BV(CS01);                // Use a /8 prescaler
  OCR0A = 49;                         // Divide by 50 (1M / 50 = 20k)

  TIMSK |= _BV(OCIE0A);              // Enable the compare match interrupt
  TIMSK &= ~_BV(TOIE0);              // Disable the overflow interrupt

}

int main (void)
{
  cli();                              // Disable inerrupts during setup
  setupPWM();
  setupTimer();
  setupADCInputs();

  lfo_wavetable = WT_SINE;
  osc_init(lfo);

  osc_set_pitch(lfo, 1);

  sei();                              // Enable interrupts again

  startADCConversion(adc_read_channel);

  while (1) {
    _delay_ms(500);
  }

  return 1;
}

ISR(ADC_vect) {
  uint16_t adc_value = ADCL;
  adc_value |= (ADCH<<8);

  switch(adc_read_channel) {
    case PARAM_1_IN_MUX:
      param_1_adc_in = adc_value;
      adc_read_channel = PARAM_2_IN_MUX;
      break;
    case PARAM_2_IN_MUX:
      param_2_adc_in = adc_value;
      adc_read_channel = PARAM_3_IN_MUX;
      break;
    case PARAM_3_IN_MUX:
      param_3_adc_in = adc_value;
      adc_read_channel = PARAM_1_IN_MUX;
      break;
    default:
      adc_read_channel = PARAM_1_IN_MUX;
  }

  startADCConversion(adc_read_channel);
}


ISR( TIM0_COMPA_vect ) {
  osc_update(lfo);

  //OCR1A = pgm_read_byte(&lfo_wavetable[osc_8bit_value(lfo)]);;
  /*OCR1A = ((MAX_ADC_VALUE - param_1_adc_in) >> 2);*/
  OCR1A = ((MAX_ADC_VALUE - param_2_adc_in) >> 2);
  /*OCR1A = (param_3_adc_in >> 2);*/
}
