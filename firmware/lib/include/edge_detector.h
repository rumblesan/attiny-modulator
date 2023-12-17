#pragma once

#include <stdbool.h>

typedef struct EdgeDetector {
  bool current_state;
  bool previous_state;
} EdgeDetector;

#define edge_detector_init(name, initial) \
  name.previous_state = initial;  \
  name.current_state = initial;

#define edge_detector_update(ed, val) \
  ed.previous_state = ed.current_state;  \
  ed.current_state = val;

#define edge_detector_is_rising(ed) (!ed.previous_state && ed.current_state)

#define edge_detector_is_falling(ed) (ed.previous_state && !ed.current_state)
