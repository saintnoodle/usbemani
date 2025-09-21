#include "idle.h"
#include "animation/ripple.h"
#include "animation/scan.h"
#include "input.h"

Idle_Timer_t idle_timer = {
    .is_idle = false,
    .last_active = {.ticks = 0},
    .timeout = IDLE_TIMEOUT_SEC * 1000,
};

void Idle_Reset(void) {
  idle_timer.is_idle = false;
  idle_timer.last_active.ticks = Timer_GetTicks();
}

void Idle_Update(Idle_Callbacks_t callbacks) {
  if (Input_HasActiveKey() || Input_IsEncoderActive()) {
    if (idle_timer.is_idle) {
      Idle_Reset();
      if (callbacks.on_reset) {
        callbacks.on_reset();
      }
    } else {
      Idle_Reset();
    }
    return;
  }

  if (!idle_timer.is_idle) {
    bool has_timeout_elapsed =
        Timer_EveryDurationInMs(&idle_timer.last_active, idle_timer.timeout);

    if (has_timeout_elapsed) {
      idle_timer.is_idle = true;
      if (callbacks.on_idle) {
        callbacks.on_idle();
      }
    } else {
      return;
    }
  }

  if (callbacks.on_calculate_frame) {
    callbacks.on_calculate_frame();
  }
}

bool Idle_IsActive(void) { return idle_timer.is_idle; }
