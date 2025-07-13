#pragma once

#include "flash.h"
#include "hardware/watchdog.h"

typedef enum {
  ResetMode_Runtime = 0,
  ResetMode_Bootloader
} _cmd_reset_mode_t;

typedef struct __attribute__((packed)) {
  _cmd_reset_mode_t mode;
  bool write;
} _cmd_reset_request_t;

static inline bool ResetCommand_Exec(CommandReport_t* command) {
  const _cmd_reset_request_t* request = (_cmd_reset_request_t*) command->request.data;

  if (request->mode == ResetMode_Runtime) {
    if (request->write) {
      Flash_Write();
    } else {
      watchdog_reboot(0, 0, 0);
    }
  }

  if (request->mode == ResetMode_Bootloader) {
    _impl_arch_resetToBootloader();
  }

  return true;
}