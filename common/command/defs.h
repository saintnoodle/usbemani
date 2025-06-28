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

// command handler function type - reads ->request and populate ->response
typedef bool (*_cmd_perform_t) (CommandReport_t* command);

// callback invoked after sending usb data - primarily useful for things like
// the reset command which would otherwise be unable to respond to the request
typedef void (*_cmd_finalize_t) (CommandReport_t* command);

typedef struct {
  uint8_t id;               // Command ID
  _cmd_perform_t perform;   // Function to perform the command
} _cmd_handler_t;