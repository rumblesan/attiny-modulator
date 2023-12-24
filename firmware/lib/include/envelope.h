#pragma once

#include <stdint.h>

#define ENV_VALUE_BITS 30
#define ENV_MAX_VALUE 1073741823 /* (1<<30) - 1 */

typedef enum {
  ENVELOPE_STOPPED,
  ENVELOPE_RISING,
  ENVELOPE_HOLDING,
  ENVELOPE_FALLING,
} EnvelopeState;

struct envelope_t {
  EnvelopeState state;
  uint8_t value_bits;
  uint32_t max_value;
  uint32_t value;
  uint32_t attack;
  uint32_t decay;
};

typedef volatile struct envelope_t Envelope;

void envelope_init(Envelope *e);

void envelope_set_attack(Envelope *e, uint32_t attack);

void envelope_set_decay(Envelope *e, uint32_t decay);

void envelope_start(Envelope *e);

void envelope_release(Envelope *e);

void envelope_tick(Envelope *e);

uint8_t envelope_8bit_value(Envelope *e);

uint8_t envelope_15bit_value(Envelope *e);

uint8_t envelope_blend_value(Envelope *e);
