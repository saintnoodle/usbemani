#include "ripple.h"

#if defined(ANIMATION_RIPPLE_ENABLED) || defined(ANIMATION_RIPPLE_IDLE_ENABLED)

AnimationRippleManager_t ripple_manager = {
    .ripples = {{0}},
    .count = 0,
    .next_slot = 0,
};

TimerTick_t last_ripple_idle_tick = {.ticks = 0};

RGB_Color_t Ripple_ClampRGB(RGB_Color_t color) {
  LIGHTING_CLAMP_RGB(color, ANIMATION_RIPPLE_MAX_BRIGHTNESS);
  return color;
}

void Ripple_Add(AnimationRippleType_t type, uint8_t origin, RGB_Color_t color,
                uint8_t speed) {
  if (origin >= LIGHTING_KEYS_COUNT) {
    return;
  }

  AnimationRipple_t *ripple = &ripple_manager.ripples[ripple_manager.next_slot];

  ripple->active = 1;
  ripple->type = type;
  ripple->origin = origin;
  ripple->step = 0;
  ripple->max_steps = 10;
  ripple->color = Ripple_ClampRGB(color);
  ripple->last_tick.ticks = Timer_GetTicks();
  ripple->step_duration = speed;

  ripple_manager.next_slot =
      (ripple_manager.next_slot + 1) % ANIMATION_RIPPLE_MAX_RIPPLES;

  if (ripple_manager.count < ANIMATION_RIPPLE_MAX_RIPPLES) {
    ripple_manager.count++;
  }
}

void Ripple_AddRandom(RGB_Color_t color, uint8_t speed) {
  uint8_t origin = Utils_Random() % LIGHTING_KEYS_COUNT;
  Ripple_Add(ANIMATION_RIPPLE_TYPE_POINT, origin, color, speed);
}

_Bool Ripple_HasActive(void) { return ripple_manager.count > 0; }

void Ripple_Clear() {
  for (uint8_t i = 0; i < ANIMATION_RIPPLE_MAX_RIPPLES; i++) {
    AnimationRipple_t *ripple = &ripple_manager.ripples[i];

    if (ripple) {
      ripple->active = 0;
    }
  }

  ripple_manager.count = 0;
  ripple_manager.next_slot = 0;
}

void Ripple_Update() {
  for (uint8_t r = 0; r < ANIMATION_RIPPLE_MAX_RIPPLES; r++) {
    AnimationRipple_t *ripple = &ripple_manager.ripples[r];

    if (!ripple->active) {
      continue;
    }

    if (Timer_EveryDurationInMs(&ripple->last_tick, ripple->step_duration)) {
      ripple->step++;

      if (ripple->step >= ripple->max_steps) {
        ripple->active = 0;
        if (ripple_manager.count > 0) {
          ripple_manager.count--;
        }
        continue;
      }
    }

    uint8_t origin_leds[4];
    uint8_t origin_led_count;
    KeyPosition_t origin_positions[4];

    if (ripple->type == ANIMATION_RIPPLE_TYPE_KEY) {
      Matrix_GetKeyLEDs(ripple->origin, origin_leds, &origin_led_count);
      for (uint8_t k = 0; k < origin_led_count; k++) {
        origin_positions[k] = Matrix_FindPosition(origin_leds[k]);
      }
    } else if (ripple->type == ANIMATION_RIPPLE_TYPE_POINT) {
      origin_led_count = 1;
      origin_leds[0] = ripple->origin;
      origin_positions[0] = Matrix_FindPosition(ripple->origin);
    }

    for (uint8_t row = 0; row < 4; row++) {
      for (uint8_t col = 0; col < 8; col++) {
        uint8_t led_index = Matrix_LEDMatrix[row][col];
        if (led_index == 255) {
          continue;
        }

        KeyPosition_t led_pos = {row, col};
        uint8_t min_distance = 255;

        for (uint8_t k = 0; k < origin_led_count; k++) {
          uint8_t distance =
              Matrix_CalculateDistance(led_pos, origin_positions[k]);
          if (distance < min_distance) {
            min_distance = distance;
          }
        }

        if (min_distance == ripple->step) {
          Lighting_BufferAdd(led_index, ripple->color);
        }
      }
    }
  }
}

void Ripple_UpdateIdle() {
  static uint16_t next_ripple_interval = ANIMATION_RIPPLE_INTERVAL_IDLE_MIN;

  if (Timer_EveryDurationInMs(&last_ripple_idle_tick, next_ripple_interval)) {

    RGB_Color_t random_color = {
        .r = (uint8_t)(Utils_Random() % (ANIMATION_RIPPLE_MAX_BRIGHTNESS + 1)),
        .g = (uint8_t)(Utils_Random() % (ANIMATION_RIPPLE_MAX_BRIGHTNESS + 1)),
        .b = (uint8_t)(Utils_Random() % (ANIMATION_RIPPLE_MAX_BRIGHTNESS + 1)),
    };

    Ripple_AddRandom(random_color, ANIMATION_RIPPLE_SPEED_IDLE);

    next_ripple_interval =
        Utils_Random() % (ANIMATION_RIPPLE_INTERVAL_IDLE_MAX -
                          ANIMATION_RIPPLE_INTERVAL_IDLE_MIN + 1) +
        ANIMATION_RIPPLE_INTERVAL_IDLE_MIN;
  }
}

#endif