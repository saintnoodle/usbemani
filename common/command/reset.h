#pragma once

#include "impl/usb.h"

typedef enum {
  ResetCommand_Bootloader = 0,
  ResetCommand_Runtime = 1
} _impl_type_t;

typedef struct __attribute__((packed)) {
  _impl_type_t type;
} _impl_request_t;

static inline bool ResetCommand_Perform(CommandReport_t* command) {
  const _impl_request_t* request = (_impl_request_t*) command->request.data;
  const _impl_type_t type = request->type;

  // NOTE: Will not send any response.
  _impl_arch_resetToBootloader();

  return false;
}