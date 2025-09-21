#pragma once

#include "defs.h"

typedef struct {
  _Bool is_idle;
  TimerTick_t last_active;
  uint16_t timeout;
} Idle_Timer_t;

typedef struct {
  void (*on_idle)(void);
  void (*on_calculate_frame)(void);
  void (*on_reset)(void);
} Idle_Callbacks_t;

extern Idle_Timer_t idle_timer;

void Idle_Reset(void);
void Idle_Update(Idle_Callbacks_t callbacks);
_Bool Idle_IsActive(void);
