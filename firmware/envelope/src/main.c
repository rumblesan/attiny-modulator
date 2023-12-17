#include <avr/io.h>

#define F_CPU 8000000

#include <avr/interrupt.h>
#include <util/delay.h>

#include "util.h"
#include "envelope.h"
#include "edge_detector.h"
#include "discrete_control.h"

#define MAX_ADC_VALUE 1023
#define DAC_CLOCK_RATE 20000

#define PARAM_1_IN_MUX 0b00001
#define PARAM_2_IN_MUX 0b00010
#define PARAM_3_IN_MUX 0b00011

#define TRIG_IN_PIN PB0
#define PWM_OUT_PIN PB1

#define LOW 0
#define HIGH 1

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

/*
// Attack/decay times in miliseconds.
const uint32_t TIME_CURVE[] = {
  0.1,
  16,
  48,
  128,
  512,
  2048,
  8192,
  16384,
  32768,
};
*/

// Attack/decay rates based on 20KHZ timer.
// ENV_MAX / (20 * time in msec)
const uint32_t TIME_CURVE[] = {
  ENV_MAX_VALUE / 2,
  ENV_MAX_VALUE / 320,
  ENV_MAX_VALUE / 960,
  ENV_MAX_VALUE / 2560,
  ENV_MAX_VALUE / 10240,
  ENV_MAX_VALUE / 40960,
  ENV_MAX_VALUE / 163840,
  ENV_MAX_VALUE / 327680,
  ENV_MAX_VALUE / 655360,
};

volatile uint8_t adc_read_channel = PARAM_1_IN_MUX;

uint16_t volatile param_1_adc_in;
uint16_t volatile param_2_adc_in;
uint16_t volatile param_3_adc_in;

volatile Envelope env;
EdgeDetector ed;

DiscreteControl attack_control;
DiscreteControl decay_control;

int main (void)
{
  disable_interrupts;
  setupPWM();
  setupTimer();
  setupADCInputs();

  DDRB &= ~_BV(TRIG_IN_PIN);

  enable_interrupts;

  startADCConversion(adc_read_channel);

  envelope_init(&env);
  envelope_set_attack(&env, 0);
  envelope_set_decay(&env, 0);

  edge_detector_init(ed, PINB & _BV(TRIG_IN_PIN));

  discrete_control_init(&attack_control, array_length(TIME_CURVE) - 1, MAX_ADC_VALUE, 2);
  discrete_control_init(&decay_control, array_length(TIME_CURVE) - 1, MAX_ADC_VALUE, 2);

  while (1) {

    edge_detector_update(ed, (PINB & _BV(TRIG_IN_PIN)));
    discrete_control_update(&attack_control, param_1_adc_in);
    discrete_control_update(&decay_control, param_2_adc_in);

    uint8_t attack_range_value = discrete_control_get_value(&attack_control);
    uint32_t attack_value = map(param_1_adc_in, 0, MAX_ADC_VALUE, TIME_CURVE[attack_range_value + 1], TIME_CURVE[attack_range_value]);
    uint8_t decay_range_value = discrete_control_get_value(&decay_control);
    uint32_t decay_value = map(param_2_adc_in, 0, MAX_ADC_VALUE, TIME_CURVE[decay_range_value + 1], TIME_CURVE[decay_range_value]);

    disable_interrupts;
    envelope_set_attack(&env, attack_value);
    envelope_set_decay(&env, decay_value);
    enable_interrupts;
    
    if (edge_detector_is_rising(ed)) {
      envelope_start(&env);
    }
    if (edge_detector_is_falling(ed)) {
      envelope_release(&env);
    }

    _delay_ms(5);
  }

  return 1;
}

ISR(ADC_vect) {
  uint16_t adc_value = ADCL;
  adc_value |= (ADCH<<8);

  switch(adc_read_channel) {
    case PARAM_1_IN_MUX:
      param_1_adc_in = MAX_ADC_VALUE - adc_value;
      adc_read_channel = PARAM_2_IN_MUX;
      break;
    case PARAM_2_IN_MUX:
      param_2_adc_in = MAX_ADC_VALUE - adc_value;
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
  envelope_tick(&env);

  OCR1A = envelope_8bit_value(&env);
}

