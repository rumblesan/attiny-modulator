#pragma once

#include <stdint.h>

#define disable_interrupts cli()

#define enable_interrupts sei()

#define array_length(a) (sizeof(a))/(sizeof(a[0]))

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

uint8_t blend(uint8_t a, uint8_t b, uint8_t blend);

uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max);
