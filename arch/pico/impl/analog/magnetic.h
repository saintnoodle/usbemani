#pragma once
#include "hardware/adc.h"

#define ANALOG_DRIVER_MAGNETIC

typedef struct {
  uint16_t min;
  uint16_t max;
  uint16_t last;
  uint8_t invert;
} _impl_magnetic_calibration_t;

extern _impl_magnetic_calibration_t _magnetic_calibration[ANALOG_CHANNELS_ACTIVE];

void _impl_analog_setCalibration(int i, uint16_t up, uint16_t down);