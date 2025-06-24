#include "usbemani.h"
#include "command.h"
#include "hardware/watchdog.h"

extern USB_LightingHelper_t USB_LightingHelper;

// When a USB command is available, invoke the relevant handler.
WEAK void CALLBACK_OnUSBCommandAvailable(CommandReport_t *command) {
  // Sample command that resets into the program or bootloader mode.
  // e.g. `hidapitester --vidpid 0573/???? --open --send-feature 255,255,1 --read-feature 255`
  if (command->request.id == 0xFF) {
    if (command->request.data[0] == 0x00) {
      _impl_arch_resetToBootloader();
    } else if (command->request.data[0] == 0x01) {
      watchdog_reboot(0, 0, 0);
    }
    return;
  }

  // Respond to everything else with some dummy response data.
  command->response.status = 0;
  command->response.data[0] = 0x13;
  command->response.data[1] = 0x37;
  command->response.data[2] = 0xCA;
  command->response.data[3] = 0xFE;
  command->ready = true;
}

// When USB output is available, setup enumerated lighting.
WEAK void CALLBACK_OnUSBOutputAvailable(USB_OutputReport_t *output) {
  USB_LightingHelper.last_output = output;
  // TODO: Make this configurable
  USB_LightingHelper.active = 4000;
}

// When no USB output is available, traditional lights should fallback to mirroring buttons 1:1
WEAK void CALLBACK_OnLightingDrawFallback(void) {
#if defined(LIGHTS_ACTIVE) && (LIGHTS_ACTIVE > 0)
  Light_SetAll(Button_GetAll());
#endif
}

// When USB output is available, traditional lights should mirror the incoming data 1:1
WEAK void CALLBACK_OnLightingDrawUSB(USB_OutputReport_t *output) {
#if defined(USB_LIGHTS_ACTIVE) && (USB_LIGHTS_ACTIVE > 0)
  Light_SetAll(output->lights);
#endif
}
