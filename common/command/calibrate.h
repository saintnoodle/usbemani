#pragma once

#include "analog.h"

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

static inline bool CalibrateGetCommand_Perform(CommandReport_t* command) {
  _cmd_calibrate_get_request_t* request = (_cmd_calibrate_get_request_t*) command->request.data;
  _cmd_calibrate_get_response_t* response = (_cmd_calibrate_get_response_t*) command->response.data;

  for (int i = 0; i < ANALOG_CHANNELS_ACTIVE; i++) {
    if (request->type == CalibrateType_Up) {
      response->value[i] = _analog_calibration[i].invert ? _analog_calibration[i].max : _analog_calibration[i].min;
    } else if (request->type == CalibrateType_Down) {
      response->value[i] = _analog_calibration[i].invert ? _analog_calibration[i].min : _analog_calibration[i].max;
    } else if (request->type == CalibrateType_Raw) {
      response->value[i] = _analog_calibration[i].last;
    }
  }

  return true;
}