#include "user.h"
#include "color/defs.h"
#include "gamma8.h"
#include <stdint.h>

// --- Buffer ---

static RGB_Color_t led_buffer[LED_BUFFER_SIZE] = {};

void bufferReset() {
  for (uint8_t i = 0; i < LED_BUFFER_SIZE; i++) {
    RGB_Color_t color = {0};
    led_buffer[i] = color;
  }
}

void bufferSet(uint8_t index, RGB_Color_t color) { led_buffer[index] = color; }

void bufferSetRange(uint8_t start, uint8_t end, RGB_Color_t color) {
  for (uint8_t i = start; i < end; i++) {
    led_buffer[i] = color;
  }
}

void bufferAdd(uint8_t index, RGB_Color_t color) {
  led_buffer[index].r += color.r;
  led_buffer[index].g += color.g;
  led_buffer[index].b += color.b;
}

void bufferSub(uint8_t index, RGB_Color_t color) {
  led_buffer[index].r -= color.r;
  led_buffer[index].g -= color.g;
  led_buffer[index].b -= color.b;
}

void bufferRender() {
  for (uint8_t i = 0; i < LED_BUFFER_SIZE; i++) {
    RGB_Set(0, i, Gamma_RGB(led_buffer[i]));
  }
  bufferReset();
}

// --- Idle ---

static IdleState_t IdleState = {.is_idle = false,
                                .last_active = {.ticks = 0},
                                .timeout = IDLE_TIMEOUT_SECS * 1000};

void idleReset(IdleState_t state) {
  state.is_idle = false;
  state.last_active.ticks = Timer_GetTicks();
}

/// Will return an enum to allow for conditional code execusion on state change
enum IdleNewState idleUpdate(IdleState_t state) {
  enum IdleNewState new_state = UNCHANGED;

  if (Encoder_Direction(0)) {
    if (state.is_idle) {
      new_state = ACTIVE;
    }

    idleReset(state);
    return new_state;
  }

  for (uint8_t i = 0; i < BUTTONS_AVAILABLE; i++) {
    if (state.is_idle) {
      new_state = ACTIVE;
    }
    if (Analog_Get(i) > 5) {
      idleReset(state);
      return new_state;
    }
  }

  if (Timer_EveryDurationInMs(&state.last_active, state.timeout) &&
      !state.is_idle) {
    new_state = IDLE;

    state.is_idle = true;
  }

  return new_state;
}

// --- Util ---

uint8_t generateRandomValueClamped(uint8_t min, uint8_t max) {
  if (max < min) {
    return min;
  }

  if (max == min) {
    return max;
  }

  uint8_t value = Utils_Random() % (max - min + 1);
  return value + min;
}

RGB_Color_t generateRandomColor(uint8_t min, uint8_t max) {
  RGB_Color_t color = {.r = generateRandomValueClamped(min, max),
                       .g = generateRandomValueClamped(min, max),
                       .b = generateRandomValueClamped(min, max)};

  return color;
}

uint8_t calculateBreathe(uint16_t phase, uint16_t max_phase) {
  if (max_phase == 0) {
    return 0;
  }

  uint32_t x = ((uint32_t)phase * 1000) / max_phase;
  uint32_t y = (4 * x * (1000 - x)) / 1000;

  y = (y * 220) / 1000;
  y += 34;

  if (y > 255) {
    y = 255;
  }

  return (uint8_t)y;
}

uint8_t scale(uint16_t value, uint8_t scale) { return (value * scale) >> 8; }

// --- Effects ---

// Assign twinkle effect state across the keys' LEDs
static TwinkleState_t TwinkleState[LED_COUNT_KEYS] = {};

void twinkleReset(TwinkleState_t state[]) {
  for (uint8_t i = 0; i < LED_COUNT_KEYS; i++) {
    state[i].color = (RGB_Color_t){};
    state[i].life = 0;
    state[i].max_life = 0;
    state[i].last_tick.ticks = Timer_GetTicks();
  }
}

void twinkleUpdate(TwinkleState_t state[]) {
  for (uint8_t i = 0; i < LED_COUNT_KEYS; i++) {
    TwinkleState_t *led = &(TwinkleState[i]);

    if (led->life > 0) {
      led->life--;

      if (led->life == 0) {
        continue;
      }

      uint8_t breathe = calculateBreathe(led->life, led->max_life);
      uint8_t brightness = Gamma(breathe);
      RGB_Color_t new_color = {.r = scale(led->color.r, brightness),
                               .g = scale(led->color.g, brightness),
                               .b = scale(led->color.b, brightness)};

      bufferAdd(i, new_color);
    } else {
      if (Utils_Random() < TWINKLE_PROBABILITY) {
        uint16_t range = TWINKLE_MAX_LIFE - TWINKLE_MIN_LIFE;
        led->max_life = TWINKLE_MIN_LIFE + (Utils_Random() & (range + 1));
        led->life = led->max_life;

        uint8_t minBrightness = 64;
        uint8_t maxBrightness = 192;

        led->color.r = generateRandomValueClamped(minBrightness, maxBrightness);
        led->color.g = generateRandomValueClamped(minBrightness, maxBrightness);
        led->color.b = generateRandomValueClamped(minBrightness, maxBrightness);
      }
    }
  }
}

// Spread the key's analog value outward from the centre LED
void analogSpread(uint8_t button, RGB_Color_t color) {
  const uint8_t value = Analog_Get(button);
  // Ignore really low values
  if (value < 5) {
    return;
  }

  // Four segments across seven LEDs
  RGB_Color_t colors[4] = {};
  const uint8_t segment_size = UINT8_MAX / 4;

  // Calculate segments
  for (uint8_t i = 0; i < 4; i++) {
    RGB_Color_t color_scaled = {};
    int16_t lower = i * segment_size;
    int16_t upper = lower + segment_size;

    if (value >= upper) {
      uint8_t brightness = segment_size * 2;

      color_scaled.r = scale(color.r, brightness);
      color_scaled.g = scale(color.g, brightness);
      color_scaled.b = scale(color.b, brightness);

      colors[i] = color_scaled;
    } else if (value > lower) {
      uint8_t brightness = (value - lower) * 2;

      color_scaled.r = scale(color.r, brightness);
      color_scaled.g = scale(color.g, brightness);
      color_scaled.b = scale(color.b, brightness);

      colors[i] = color_scaled;
    } else {
      colors[i] = color_scaled;
    }
  }

  // Place values into buffer
  const uint8_t led_start = button * CONTROLLER_RGB_LEDS_PER_KEY;

  // Start with centre LED
  bufferSet(led_start + 3, colors[0]);

  bufferSet(led_start + 2, colors[1]);
  bufferSet(led_start + 4, colors[1]);

  bufferSet(led_start + 1, colors[2]);
  bufferSet(led_start + 5, colors[2]);

  bufferSet(led_start, colors[3]);
  bufferSet(led_start + 6, colors[3]);
}

// --- Callbacks ---

void CALLBACK_OnHardwareReady() {
  bufferReset();
  twinkleReset(TwinkleState);
}

void CALLBACK_RGBCalculateNextFrame() {
  enum IdleNewState idle_state_update = idleUpdate(IdleState);

  if (idle_state_update == ACTIVE) {
    twinkleReset(TwinkleState);
  }

  if (IdleState.is_idle) {
    // Run idle effects
    twinkleUpdate(TwinkleState);
  } else {
    // Run analog spread effect across keys
    for (uint8_t i = 0; i < LED_COUNT_PLAY; i++) {
      analogSpread(i, (RGB_Color_t){.r = 192, .g = 192, .b = 192});
    }

    // Run analog spread effect across effect keys
    for (uint8_t i = 0; i < LED_COUNT_EFFECT; i++) {
      analogSpread(i + KEY_INDEX_START_EFFECT,
                   (RGB_Color_t){.r = 192, .g = 192, .b = 192});
    }

    // Set TT to a static colour
    bufferSetRange(LED_INDEX_START_TT, CONTROLLER_RGB_LEDS_TURNTABLE,
                   (RGB_Color_t){.r = 255, .g = 255, .b = 255});
  }
}

void CALLBACK_OnRGBDrawFallback() {
  RGB_ClearAll(0);

  bufferRender();
}

void CALLBACK_OnRGBDrawUSB(USB_OutputReport_t *output) {
  RGB_ClearAll(0);

  for (int i = 0; i < KEY_COUNT_PLAY; i++) {
    RGB_SetRange(0, (i * CONTROLLER_RGB_LEDS_PER_KEY),
                 CONTROLLER_RGB_LEDS_PER_KEY, output->rgb[i]);
  }

  for (uint8_t i = 0; i < KEY_COUNT_EFFECT; i++) {
    const uint8_t position =
        LED_INDEX_START_EFFECT + (i * CONTROLLER_RGB_LEDS_PER_KEY);
    RGB_SetRange(0, position, CONTROLLER_RGB_LEDS_PER_KEY, output->rgb[7 + i]);
  }

  // Draw TT
  RGB_SetRange(0, LED_INDEX_START_TT, CONTROLLER_RGB_LEDS_TURNTABLE,
               output->rgb[7 + 4]);
}
