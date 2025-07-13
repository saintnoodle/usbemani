#pragma once

#include "impl/flash.h"

static inline void Flash_Init(void) {
  _impl_flash_init();
}

static inline bool Flash_Read(void) {
  return _impl_flash_read();
}

static inline bool Flash_Write(void) {
  return _impl_flash_write();
}