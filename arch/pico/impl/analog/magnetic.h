#pragma once

#define ANALOG_DRIVER_MAGNETIC

#include "settings.h"
#include "hardware/adc.h"

typedef struct {
  uint16_t raw;
} _impl_magnetic_t;

extern _impl_magnetic_t _magnetic[ANALOG_CHANNELS_ACTIVE];

void _impl_analog_setCalibration(int i, uint16_t up, uint16_t down);