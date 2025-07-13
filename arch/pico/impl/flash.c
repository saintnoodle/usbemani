#include <string.h>
#include "config.h"
#include "settings.h"

#include "hardware/sync.h"
#include "hardware/flash.h"
#include "hardware/watchdog.h"

static inline uint8_t* _impl_flash_addr(void) {
  return (uint8_t*) (XIP_BASE + FLASH_TARGET_OFFSET);
}

void _impl_flash_init(void) {
  //
  // Check if the settings structure is already populated. Values are persisted
  // to flash by calling the 'commit' USB command, which sets the dirty flag and
  // reboots the device.
  //
  // This is mostly done because I'm not sure how to write to flash while the
  // controller is actively running without breaking things. It's easier to do
  // it now while nothing is running yet.
  //
  // If this is a regular boot, just copy the default settings instead.
  //
  if (!watchdog_caused_reboot()) {
    return;
  }

  // Prepare to copy settings from RAM to flash.
  _settings.dirty = false;

  uint8_t buffer[FLASH_SECTOR_SIZE] = {0};
  memcpy(buffer, &_settings, sizeof(Settings_t));

  const uint32_t interrupts = save_and_disable_interrupts();

  flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
  flash_range_program(FLASH_TARGET_OFFSET, buffer, FLASH_SECTOR_SIZE);

  restore_interrupts(interrupts);
}

bool _impl_flash_read(void) {
  //
  // If the current settings aren't valid, initialize defaults.
  //
  if (_settings.magic != SETTINGS_MAGIC) {
    Settings_Init();
  }

  //
  // Ensure the existing data in flash is valid before copying.
  //
  const Settings_t* src = (Settings_t*) _impl_flash_addr();

  if (src->magic != SETTINGS_MAGIC) {
    return false;
  }

  memcpy(&_settings, src, sizeof(Settings_t));

  return true;
}

bool _impl_flash_write(void) {
  //
  // Set the dirty flag and reboot.
  //
  _settings.dirty = true;

  watchdog_reboot(0, 0, 0);

  return true;
}