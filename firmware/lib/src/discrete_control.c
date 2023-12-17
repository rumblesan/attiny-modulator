#include "util.h"

#include "discrete_control.h"

void discrete_control_init(DiscreteControl *dc, uint16_t num_values, uint16_t range, uint8_t hysteresis) {
  dc->current_value = 0;
  dc->previous_value = 0;

  dc->raw_value = 0;
  dc->step = range / constrain(num_values, 1, 256);
  dc->range = range;
  dc->max_value = num_values - 1;
  dc->hysteresis = constrain(hysteresis, 0, dc->step / 2);

  discrete_control_update_thresholds(dc);
}

void discrete_control_set_num_values(DiscreteControl *dc, uint16_t num_values) {
  num_values = constrain(num_values, 1, 256);

  dc->max_value = num_values - 1;
  dc->step = dc->range / num_values;
  dc->current_value = dc->raw_value / dc->step;
  discrete_control_update_thresholds(dc);
}
void discrete_control_set_hysteresis(DiscreteControl *dc, uint8_t hysteresis) {
  dc->hysteresis = constrain(hysteresis, 0, dc->step / 2);

  discrete_control_update_thresholds(dc);
}


void discrete_control_update_thresholds(DiscreteControl *dc) {
  uint16_t current = dc->current_value;
  uint16_t lower = current * dc->step;

  if (current == 0) {
    dc->lower_threshold = 0;
  } else {
    dc->lower_threshold = lower - dc->hysteresis;
  }

  if (current == dc->max_value) {
    dc->upper_threshold = dc->range;
  } else {
    dc->upper_threshold = lower + dc->step + dc->hysteresis;
  }
}

void discrete_control_update(DiscreteControl *dc, uint16_t value) {
  dc->raw_value = value;

  if (value < dc->lower_threshold || value > dc->upper_threshold) {
    dc->previous_value = dc->current_value;
    dc->current_value = value / dc->step;
    discrete_control_update_thresholds(dc);
  }
}
