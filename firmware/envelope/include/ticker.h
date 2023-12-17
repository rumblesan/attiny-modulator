#pragma once

#include <stdint.h>

typedef struct Ticker {
  volatile uint16_t count;
} Ticker;

#define ticker_init(ticker) \
  (ticker)->count = 0;

#define ticker_set(ticker, value) \
  (ticker)->count = value;

#define ticker_increment(ticker, incr) \
  (ticker)->count += incr;

#define ticker_reset(ticker) \
  (ticker)->count = 0;

#define ticker_tick(ticker) \
  (ticker)->count = (ticker)->count + 1;

#define ticker_count(ticker) (ticker)->count

#define ticker_8bit_count(ticker) ((ticker)->count >> 8)
