#pragma once
#include "hardware/adc.h"

typedef struct {
  uint16_t min;
  uint16_t max;
  uint16_t last;
  uint8_t  invert;
} _impl_calibration_t;

extern _impl_calibration_t _analog_calibration[ANALOG_CHANNELS_ACTIVE];