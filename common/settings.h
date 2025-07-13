#pragma once

#include <stdint.h>
#include <stdbool.h>

#define SETTINGS_MAGIC 0x1337d00d

typedef struct {
  uint16_t min[ANALOG_CHANNELS_ACTIVE];
  uint16_t max[ANALOG_CHANNELS_ACTIVE];
  uint16_t up_threshold[ANALOG_CHANNELS_ACTIVE];   // Point at which to deactivate the button
  uint16_t down_threshold[ANALOG_CHANNELS_ACTIVE]; // Point at which to activate the button
  uint16_t invert;                                 // Bit flags for inverted channels
} _settings_analog_t;

typedef struct __attribute__((packed)) {
  uint32_t magic;
  _settings_analog_t analog;
  bool dirty;
} Settings_t;

extern Settings_t __attribute__((section(".noinit"))) _settings;

static inline void Settings_Init(void) {
  memset(&_settings, 0, sizeof(Settings_t));
  _settings.magic = SETTINGS_MAGIC;
}