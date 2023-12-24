#include "envelope.h"

void envelope_init(Envelope *e) {
  e->state = ENVELOPE_STOPPED;
  e->value = 0;
  e->attack = 0;
  e->decay = 0;
}

void envelope_set_attack(Envelope *e, uint32_t attack) {
  e->attack = attack;
}

void envelope_set_decay(Envelope *e, uint32_t decay) {
  e->decay = decay;
}

void envelope_start(Envelope *e) {
  e->state = ENVELOPE_RISING;
}

void envelope_release(Envelope *e) {
  e->state = ENVELOPE_FALLING;
}

void envelope_tick(Envelope *e) {
  switch(e->state) {
    case ENVELOPE_RISING:
      if ((e->value + e->attack) > ENV_MAX_VALUE) {
        // overflow would occur
        e->value = ENV_MAX_VALUE;
        e->state = ENVELOPE_HOLDING;
      } else {
        e->value = e->value + e->attack;
      }
      break;
    case ENVELOPE_FALLING:
      if ((e->value - e->decay) > e->value) {
        // overflow would occur
        e->value = 0;
        e->state = ENVELOPE_STOPPED;
      } else {
        e->value = e->value - e->decay;
      }
      break;
    default:
      break;
  }
}

uint8_t envelope_8bit_value(Envelope *e) {
  return e->value >> 22;
}

// designed to work with the blend function
uint8_t envelope_blend_value(Envelope *e) {
  uint16_t bits16val = e->value >> 15;
  return (bits16val & 127);
}
