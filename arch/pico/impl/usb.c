#include "usb.h"
#include "usb/descriptors.c"

void _impl_hid_init(void) {
  tusb_init();
}

void _impl_hid_inputReportKonamiCloud(void) {
#if defined(KONAMI_CLOUD_ENABLED)
  static USB_InputReport_KonamiCloud_t input;

  memset(&input, 0, sizeof(input));
  CALLBACK_OnKonamiCloudInputRequest(&input);
  tud_hid_report(ReportID_Input, &input, sizeof(input));
#endif
}

void _impl_hid_inputReportUSBemani(void) {
  static USB_InputReport_USBemani_t input;

  memset(&input, 0, sizeof(input));
  CALLBACK_OnUSBemaniInputRequest(&input);
  tud_hid_report(ReportID_Input, &input, sizeof(input));
}

// Sends input report data to the host.
uint16_t _impl_hid_handleInputReport(uint8_t *buffer) {
  if (_usb_status.mode == USB_DeviceType_KonamiCloud) {
#if defined(KONAMI_CLOUD_ENABLED)
    USB_InputReport_KonamiCloud_t *input = (USB_InputReport_KonamiCloud_t *)buffer;
    CALLBACK_OnKonamiCloudInputRequest(input);
    return sizeof(USB_InputReport_KonamiCloud_t);
#endif
  } else {
    USB_InputReport_USBemani_t *input = (USB_InputReport_USBemani_t *)buffer;
    CALLBACK_OnUSBemaniInputRequest(input);
    return sizeof(USB_InputReport_USBemani_t);
  }
}

// Sends the response to a previously received command feature report.
uint16_t _impl_hid_handleFeatureReport(uint8_t *buffer) {
  if (!_command.ready) {
    return 0;
  }

  static const uint16_t size = sizeof(USB_CommandResponse_t);
  memcpy(buffer, &_command.response, size);

  _command.ready = false;
  memset(&_command.request, 0, sizeof(USB_CommandRequest_t));
  memset(&_command.response, 0, sizeof(USB_CommandResponse_t));

  return size;
}

// Called when an output report is received from the host.
void _impl_hid_setOutputReport(uint8_t const *buffer, uint16_t bufsize) {
  if (bufsize == sizeof(USB_OutputReport_t)) {
    static USB_OutputReport_t output;
    memcpy(&output, buffer, sizeof(USB_OutputReport_t));
    CALLBACK_OnUSBOutputAvailable(&output);
  }
}

// Called when a command feature report is received from the host.
void _impl_hid_setFeatureReport(uint8_t const *buffer, uint16_t bufsize) {
  if (bufsize == sizeof(USB_CommandRequest_t)) {
    memcpy(&_command.request, buffer, sizeof(USB_CommandRequest_t));
    memset(&_command.response, 0, sizeof(USB_CommandResponse_t));
    CALLBACK_OnUSBCommandAvailable(&_command);
  }
}

// Standard task for building input reports.
// TinyUSB appears to handle output reports using the same handler as the output ControlRequest handler?
void _impl_hid_dataHandlerTask(void) {
  if (tud_suspended())
    tud_remote_wakeup();


  if (tud_hid_ready()) {
    if (_usb_status.mode == USB_DeviceType_KonamiCloud)
      _impl_hid_inputReportKonamiCloud();
    else
      _impl_hid_inputReportUSBemani();
  }
}

// TinyUSB event handler for USB_Connect events.
void tud_mount_cb(void) { }

// TinyUSB event handler for USB_Disconnect events.
void tud_umount_cb(void) { }

// TinyUSB event handler for USB_Suspend events.
void tud_suspend_cb(bool remote_wakeup_en) { }

// TinyUSB event handler for USB_WakeUp events.
void tud_resume_cb(void) { }

//// TinyUSB event handlers for USB_ControlRequest events.
// Fill the buffer report and return the length.
// TODO:  This code might not be needed. Most examples return 0.
//        This is populated because LUFA handles ControlRequest events in this fashion.
uint16_t tud_hid_get_report_cb(
  uint8_t instance,
  uint8_t report_id,
  hid_report_type_t report_type,
  uint8_t *buffer,
  uint16_t reqlen
) {
  if (report_type == HID_REPORT_TYPE_INPUT) {
    return _impl_hid_handleInputReport(buffer);
  } else if (report_type == HID_REPORT_TYPE_FEATURE) {
    return _impl_hid_handleFeatureReport(buffer);
  }

  return 0;
}

// Receive and process reports.
void tud_hid_set_report_cb(
  uint8_t instance,
  uint8_t report_id,
  hid_report_type_t report_type,
  uint8_t const *buffer,
  uint16_t bufsize
) {
  // TODO:  HID reports work if we check `report_type` to confirm an output report.
  //        However, this only works for SET_REPORT requests and not URB Interrupt Outs.
  //        Why does this differ?
  if (report_type == HID_REPORT_TYPE_OUTPUT) {
    // Strip report ID before passing to the output handler.
    _impl_hid_setOutputReport(++buffer, --bufsize);
  } else if (report_type == HID_REPORT_TYPE_FEATURE) {
    _impl_hid_setFeatureReport(buffer, bufsize);
  }
}

const uint8_t *tud_descriptor_device_cb(void) {
  return (const uint8_t *)(_usb_status.device_descriptor);
}

const uint8_t *tud_hid_descriptor_report_cb(uint8_t index) {
  return (const uint8_t *)(_usb_status.report_descriptor);
}

const uint8_t *tud_descriptor_configuration_cb(uint8_t index) {
  return (const uint8_t *)(_usb_status.config_descriptor);
}

const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  const USB_STRING   *string = NULL;
  register uint8_t stridx;

  uint8_t type = index & StringTypeMask;
  if (type & StringType_LightsRGB) {
    type   = StringType_LightsRGB;
    stridx = index & StringIndexMask_RGB;
  } else {
    stridx = index & StringIndexMask;
  }

  switch(type) {
  case StringType_USBStandard:
    string = String_USB[stridx];
    break;
  case StringType_Button:
    string = String_Button[stridx];
    break;
  case StringType_Encoder:
    string = String_Encoder[stridx];
    break;
  case StringType_Axis:
    string = String_Axis[stridx];
    break;
  case StringType_LightsBasic:
    string = String_Light[stridx];
    break;
  case StringType_LightsRGB:
    string = String_RGB[stridx];
    break;
  }
  return (const uint16_t *)__builtin_assume_aligned(string, 2);
}
