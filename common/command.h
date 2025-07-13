#pragma once

#include "command/defs.h"
#include "command/reset.h"

#if defined(ANALOG_DRIVER_MAGNETIC)
#include "command/calibrate.h"
#endif

/*** Declarations ***/
// Storage for the current command report
extern CommandReport_t _command;

static const _cmd_handler_t handlers[] = {
#if defined(ANALOG_DRIVER_MAGNETIC)
  { .id = 0x01, .exec = CalibrateGetCommand_Exec },
  { .id = 0x02, .exec = CalibrateSetCommand_Exec },
#endif
  { .id = 0xFF, .exec = ResetCommand_Exec },
};

static inline const _cmd_handler_t* Command_Find(uint8_t id) {
  static const int count = sizeof(handlers) / sizeof(_cmd_handler_t);
  for (int i = 0; i < count; i++) {
    if (handlers[i].id == id) {
      return handlers + i;
    }
  }
  return NULL;
}

static inline void Command_Run(CommandReport_t* command) {
  const _cmd_handler_t* handler = Command_Find(command->request.id);
  if (handler == NULL) {
    command->response.status = CommandResponseStatus_Unknown;
  } else if (!handler->exec(command)) {
    command->response.status = CommandResponseStatus_Failure;
  } else {
    command->response.status = CommandResponseStatus_Success;
  }
  command->ready = true;
}