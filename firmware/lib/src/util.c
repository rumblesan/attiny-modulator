#include "util.h"

#include <stdint.h>

uint8_t blend(uint8_t a, uint8_t b, uint8_t blend)
{
  if (blend > 128)
    blend = 128;
  // Guaranteed to not overflow. UINT8_MAX * 64 + UINT8_MAX * 64 == 2^15 - 1
  uint16_t aScaled = a * (128-blend);
  uint16_t bScaled = b * blend;
  return (aScaled + bScaled) >> 7;
}

uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max) {
  uint32_t step = (out_max - out_min) / (in_max - in_min);
  return ((x - in_min) * step) + out_min;
}
