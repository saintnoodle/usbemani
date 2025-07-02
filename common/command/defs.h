#pragma once

#include "usb/descriptors/usbemani.h"

typedef enum {
  CommandStatus_None     = 0x00, // Command report is not ready
  CommandStatus_Complete = 0x01, // Response is ready to be sent back to host
} CommandStatus_t;

typedef enum {
  CommandResponseStatus_Success = 0x00, // Command was successfully handled
  CommandResponseStatus_Unknown = 0x01, // Command was not found
  CommandResponseStatus_Failure = 0x02, // Command was handled but failed
} CommandResponseStatus_t;

typedef struct {
  bool ready;
  USB_CommandRequest_t request;   // Request received from the host
  USB_CommandResponse_t response; // Response to send back to the host
} CommandReport_t;

typedef bool (*_cmd_exec_t) (CommandReport_t* command);

typedef struct {
  uint8_t id;
  _cmd_exec_t exec;
} _cmd_handler_t;