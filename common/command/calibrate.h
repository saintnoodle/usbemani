#pragma once

#include "analog.h"
#include "settings.h"

typedef enum {
  CalibrateType_Up = 0,
  CalibrateType_Down,
  CalibrateType_Raw
} _cmd_calibrate_type_t;

typedef struct __attribute__((packed)) {
  _cmd_calibrate_type_t type;
} _cmd_calibrate_get_request_t;

typedef struct __attribute__((packed)) {
  uint16_t value[ANALOG_CHANNELS_ACTIVE];
} _cmd_calibrate_get_response_t;

static inline bool CalibrateGetCommand_Exec(CommandReport_t* command) {
  _cmd_calibrate_get_request_t* request = (_cmd_calibrate_get_request_t*) command->request.data;
  _cmd_calibrate_get_response_t* response = (_cmd_calibrate_get_response_t*) command->response.data;

  for (int i = 0; i < ANALOG_CHANNELS_ACTIVE; i++) {
    const bool inverted = (_settings.analog.invert >> i) & 1;
    if (request->type == CalibrateType_Up) {
      response->value[i] = inverted ? _settings.analog.max[i] : _settings.analog.min[i];
    } else if (request->type == CalibrateType_Down) {
      response->value[i] = inverted ? _settings.analog.min[i] : _settings.analog.max[i];
    } else if (request->type == CalibrateType_Raw) {
      response->value[i] = _magnetic[i].raw;
    }
  }

  return true;
}

typedef struct __attribute__((packed)) {
  uint8_t channel;
  uint16_t up;
  uint16_t down;
} _cmd_calibrate_set_request_t;

static inline bool CalibrateSetCommand_Exec(CommandReport_t* command) {
  const _cmd_calibrate_set_request_t* request = (_cmd_calibrate_set_request_t*) command->request.data;

  // Ensure the channel is valid.
  if (request->channel >= ANALOG_CHANNELS_ACTIVE) {
    return false;
  }

  // Set the calibration values.
  _impl_analog_setCalibration(
    request->channel,
    request->up,
    request->down
  );

  return true;
}