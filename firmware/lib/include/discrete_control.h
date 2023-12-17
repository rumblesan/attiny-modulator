#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct DiscreteControl {
  uint8_t current_value;
  uint8_t previous_value;

  uint16_t raw_value;
  uint8_t hysteresis;
  uint16_t step;
  uint16_t max_value;
  uint16_t range;
  uint16_t upper_threshold;
  uint16_t lower_threshold;

} DiscreteControl;

void discrete_control_init(DiscreteControl *dc, uint16_t num_values, uint16_t range, uint8_t hysteresis);

void discrete_control_set_hysteresis(DiscreteControl *dc, uint8_t hysteresis);

void discrete_control_update_thresholds(DiscreteControl *dc);

void discrete_control_update(DiscreteControl *dc, uint16_t value);

#define discrete_control_get_value(dc) (dc)->current_value
#define discrete_control_get_raw_value(dc) (dc)->raw_value
