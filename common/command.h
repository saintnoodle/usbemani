#pragma once

#include "usb/descriptors/usbemani.h"

typedef struct {
  bool ready;                     // Whether a response is waiting to be sent
  USB_CommandRequest_t request;   // Request received from the host
  USB_CommandResponse_t response; // Response to send back to the host
} CommandReport_t;

extern CommandReport_t _command;