#pragma once

#include "button.h"
#include "encoder.h"

typedef struct __attribute__((packed)) {
#if defined(USB_ENCODER_AXES_ACTIVE) && (USB_ENCODER_AXES_ACTIVE > 0)
  uint16_t  encoder[USB_ENCODER_AXES_ACTIVE];
#endif
#if defined(USB_GENERAL_AXES_ACTIVE) && (USB_GENERAL_AXES_ACTIVE > 0)
   int8_t   axis[USB_GENERAL_AXES_ACTIVE];
#endif
#if defined(USB_BUTTONS_ACTIVE) && (USB_BUTTONS_ACTIVE > 0)
  uint16_t  buttons;
#endif
} USB_InputReport_USBemani_t;

typedef struct __attribute__((packed)) {
  uint8_t id;
  uint8_t data[USB_CMD_REPORT_SIZE - 2];
} USB_CommandRequest_t;

typedef struct __attribute__((packed)) {
  uint8_t status;
  uint8_t data[USB_CMD_REPORT_SIZE - 2];
} USB_CommandResponse_t;