#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Oscillator {
  uint16_t phase;
  uint16_t phase_delta;
  uint16_t phase_offset;
} Oscillator;

#define osc_init(osc) \
  osc.phase = 0;  \
  osc.phase_delta = 0;  \
  osc.phase_offset = 0;

#define osc_update(osc) osc.phase = osc.phase + osc.phase_delta + osc.phase_offset;

#define osc_set_pitch(osc, delta) osc.phase_delta = delta;

// 0 to 1023 gives four octaves of range
#define osc_set_phase_offset(osc, offset) osc.phase_offset = offset;

#define osc_value(osc) osc.phase

#define osc_8bit_value(osc) (osc.phase >> 8)
