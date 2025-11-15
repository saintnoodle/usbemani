#include "user.h"

// --- Colours ---

static const RGB_Color_t COLOR_NONE = {0};
static const RGB_Color_t COLOR_WHITE_FULL = {.r = 255, .g = 255, .b = 255};
static const RGB_Color_t COLOR_WHITE_HALF = {.r = 128, .g = 128, .b = 128};

static const RGB_Color_t COLOR_TT = COLOR_WHITE_FULL;
static const RGB_Color_t COLOR_KEYS_PLAY = COLOR_WHITE_HALF;
static const RGB_Color_t COLOR_KEYS_EFFECT = {.r = 128, .g = 8, .b = 64};

// --- Buffer ---

static RGB_Color_t led_buffer[LED_BUFFER_SIZE];

static void bufferInit() {
  for (uint8_t i = 0; i < LED_BUFFER_SIZE; i++) {
    led_buffer[i] = COLOR_NONE;
  }
}

// void bufferSet(uint8_t index, RGB_Color_t color) { led_buffer[index] = color;
// }

// void bufferSetRange(uint8_t start, uint8_t count, RGB_Color_t color) {
//   for (uint8_t i = start; i < count; i++) {
//     led_buffer[i] = color;
//   }
// }

static void bufferAdd(uint8_t index, RGB_Color_t color) {
  uint16_t new_r = led_buffer[index].r + color.r;
  uint16_t new_g = led_buffer[index].g + color.g;
  uint16_t new_b = led_buffer[index].b + color.b;

  led_buffer[index].r = (new_r > 255) ? 255 : new_r;
  led_buffer[index].g = (new_g > 255) ? 255 : new_g;
  led_buffer[index].b = (new_b > 255) ? 255 : new_b;
}

static void bufferAddRange(uint8_t start, uint8_t count, RGB_Color_t color) {
  for (uint8_t i = 0; i < count; i++) {
    uint8_t index = start + i;

    uint16_t new_r = led_buffer[index].r + color.r;
    uint16_t new_g = led_buffer[index].g + color.g;
    uint16_t new_b = led_buffer[index].b + color.b;

    led_buffer[index].r = (new_r > 255) ? 255 : new_r;
    led_buffer[index].g = (new_g > 255) ? 255 : new_g;
    led_buffer[index].b = (new_b > 255) ? 255 : new_b;
  }
}

static void bufferRender() {
  for (uint8_t i = 0; i < LED_COUNT_KEYS; i++) {
    RGB_Color_t color = led_buffer[i];
    RGB_Color_t clamped_color = {
        .r = MIN(color.r, 192), .g = MIN(color.g, 192), .b = MIN(color.b, 192)};

    RGB_Set(0, i, clamped_color);
  }

  for (uint8_t i = 0; i < CONTROLLER_RGB_LEDS_TURNTABLE; i++) {
    RGB_Set(0, i + LED_INDEX_START_TT, led_buffer[i + LED_INDEX_START_TT]);
  }
}

// --- Gamma ---

static const uint8_t gamma8[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,
    1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,
    2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,
    4,   5,   5,   5,   5,   6,   6,   6,   6,   7,   7,   7,   7,   8,   8,
    8,   9,   9,   9,   10,  10,  10,  11,  11,  11,  12,  12,  13,  13,  13,
    14,  14,  15,  15,  16,  16,  17,  17,  18,  18,  19,  19,  20,  20,  21,
    21,  22,  22,  23,  24,  24,  25,  25,  26,  27,  27,  28,  29,  29,  30,
    31,  32,  32,  33,  34,  35,  35,  36,  37,  38,  39,  39,  40,  41,  42,
    43,  44,  45,  46,  47,  48,  49,  50,  50,  51,  52,  54,  55,  56,  57,
    58,  59,  60,  61,  62,  63,  64,  66,  67,  68,  69,  70,  72,  73,  74,
    75,  77,  78,  79,  81,  82,  83,  85,  86,  87,  89,  90,  92,  93,  95,
    96,  98,  99,  101, 102, 104, 105, 107, 109, 110, 112, 114, 115, 117, 119,
    120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142, 144, 146,
    148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175, 177,
    180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
    215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252,
    255};

static uint8_t gammaCorrect(uint8_t value) { return gamma8[value]; };
// static RGB_Color_t gammaCorrectRGB(RGB_Color_t color) {
//   return (RGB_Color_t){
//       .r = gamma8[color.r], .g = gamma8[color.g], .b = gamma8[color.b]};
// };

// --- Idle ---

static IdleState_t IdleState = {.is_idle = false,
                                .last_active = {.ticks = 0},
                                .timeout = IDLE_TIMEOUT_SECS * 1000};

static void idleReset() {
  IdleState.is_idle = false;
  IdleState.last_active.ticks = Timer_GetTicks();
}

/// Will return an enum to allow for conditional code execusion on state change
static enum IdleNewState idleUpdate() {
  enum IdleNewState new_state = UNCHANGED;
  bool activity = false;

  for (uint8_t i = 0; i < BUTTONS_AVAILABLE; i++) {
    if (Analog_Get(i) > 5) {
      activity = true;
      break;
    }
  }

  if (!activity && Encoder_Direction(0)) {
    activity = true;
  }

  if (activity) {
    if (IdleState.is_idle) {
      new_state = ACTIVE;
    }
    idleReset();
  }

  if (!IdleState.is_idle) {
    if (Timer_EveryDurationInMs(&IdleState.last_active, IdleState.timeout)) {
      IdleState.is_idle = true;
      new_state = IDLE;
    }
  }

  return new_state;
}

// --- Util ---

static uint8_t generateRandomValueClamped(uint8_t min, uint8_t max) {
  if (max < min) {
    return min;
  }

  if (max == min) {
    return max;
  }

  uint8_t value = Utils_Random() % (max - min + 1);
  return value + min;
}

static RGB_Color_t generateRandomColor(uint8_t min, uint8_t max) {
  return (RGB_Color_t){.r = generateRandomValueClamped(min, max),
                       .g = generateRandomValueClamped(min, max),
                       .b = generateRandomValueClamped(min, max)};
}

static uint8_t calculateBreathe(uint16_t phase, uint16_t max_phase) {
  if (max_phase == 0) {
    return 0;
  }

  uint32_t x = ((uint32_t)phase * 1000) / max_phase;
  uint32_t y = (4 * x * (1000 - x)) / 1000;

  y = (y * 186) / 1000;
  y += 34;

  return y > 220 ? 220 : (uint8_t)y;
}

static uint8_t scale(uint16_t value, uint8_t scale) {
  return (value * scale) >> 8;
}

// --- Effects ---

// Assign twinkle effect state across the keys' LEDs
static TwinkleState_t TwinkleState[LED_COUNT_KEYS];

static void twinkleReset() {
  for (uint8_t i = 0; i < LED_COUNT_KEYS; i++) {
    TwinkleState[i].color = (RGB_Color_t){0};
    TwinkleState[i].life = 0;
    TwinkleState[i].max_life = 0;
    TwinkleState[i].last_tick.ticks = Timer_GetTicks();
  }
}

static void twinkleUpdate() {
  for (uint8_t i = 0; i < LED_COUNT_KEYS; i++) {
    TwinkleState_t *led = &TwinkleState[i];

    if (led->life > 0) {
      led->life--;

      if (led->life == 0) {
        continue;
      }

      uint8_t breathe = calculateBreathe(led->life, led->max_life);
      uint8_t brightness = gammaCorrect(breathe);
      RGB_Color_t new_color = {.r = scale(led->color.r, brightness),
                               .g = scale(led->color.g, brightness),
                               .b = scale(led->color.b, brightness)};

      if (brightness < 32 &&
          (new_color.r > 0 || new_color.g > 0 || new_color.b > 0)) {
        if (new_color.r < 1 && led->color.r > 0)
          new_color.r = 1;
        if (new_color.g < 1 && led->color.g > 0)
          new_color.g = 1;
        if (new_color.b < 1 && led->color.b > 0)
          new_color.b = 1;
      }

      bufferAdd(i, new_color);
    } else {
      if (Utils_Random() < TWINKLE_PROBABILITY) {
        uint16_t range = TWINKLE_MAX_LIFE - TWINKLE_MIN_LIFE;
        led->max_life = TWINKLE_MIN_LIFE + (Utils_Random() & (range + 1));
        led->life = led->max_life;

        uint8_t minBrightness = 48;
        uint8_t maxBrightness = 220;

        led->color = generateRandomColor(minBrightness, maxBrightness);
      }
    }
  }
}

static SweepState_t SweepState = {
    .active_mode = true,
    .pos = 0,
    .loop_duration = RGB_FRAMERATE_TARGET * 5,
    .direction = CW,
    .led_start_pos = 96,
};

static uint8_t sweepGetRealIndex(uint8_t index) {
  if (SweepState.direction == CW) {
    return (CONTROLLER_RGB_LEDS_TURNTABLE - 1 - index +
            (SweepState.led_start_pos * CONTROLLER_RGB_LEDS_TURNTABLE / 256)) %
           CONTROLLER_RGB_LEDS_TURNTABLE;
  } else {
    return (index +
            (SweepState.led_start_pos * CONTROLLER_RGB_LEDS_TURNTABLE / 256)) %
           CONTROLLER_RGB_LEDS_TURNTABLE;
  }
}

static uint8_t sweepGetLEDBrightness(uint16_t progress, uint16_t led_start,
                                     uint16_t led_duration, bool reverse) {
  if (progress >= led_start + led_duration) {
    return reverse ? 0 : 255;
  }

  if (progress >= led_start) {
    uint16_t led_progress = progress - led_start;
    uint8_t led_progress_normalised = (led_progress * 255) / led_duration;

    if (reverse) {
      return gammaCorrect(255 - led_progress_normalised);
    }

    return gammaCorrect(led_progress_normalised);
  }

  return reverse ? 255 : 0;
}

static void sweepReset(bool wake) {
  if (wake) {
    SweepState.active_mode = true;
  } else {
    SweepState.active_mode = false;
    SweepState.pos = RGB_FRAMERATE_TARGET * 2.5;
  }
}

static void sweepRun(uint16_t progress, uint16_t led_duration, bool reverse) {
  for (uint8_t i = 0; i < CONTROLLER_RGB_LEDS_TURNTABLE; i++) {
    uint8_t effective_i = sweepGetRealIndex(i);
    uint16_t led_start = i * led_duration;

    uint8_t brightness =
        sweepGetLEDBrightness(progress, led_start, led_duration, reverse);

    if (brightness > 0) {
      bufferAdd(LED_INDEX_START_TT + effective_i,
                (RGB_Color_t){.r = scale(COLOR_TT.r, brightness),
                              .g = scale(COLOR_TT.g, brightness),
                              .b = scale(COLOR_TT.b, brightness)});
    }
  }
}

static void sweepUpdate() {
  uint8_t phase = SweepState.pos / (SweepState.loop_duration / 4);
  uint32_t phase_duration = SweepState.loop_duration / 4;
  uint32_t phase_progress = SweepState.pos % phase_duration;

  uint32_t led_duration = (phase_duration / CONTROLLER_RGB_LEDS_TURNTABLE) / 2;

  switch (phase) {
  case 0: {
    sweepRun(phase_progress, led_duration, false);
    break;
  }
  case 1: {
    // Simply turn all LEDs on
    bufferAddRange(LED_INDEX_START_TT, CONTROLLER_RGB_LEDS_TURNTABLE, COLOR_TT);
    if (SweepState.active_mode) {
      // stop progressing the animation if we're active
      return;
    }
    break;
  }
  case 2: {
    sweepRun(phase_progress, led_duration, true);
    break;
  }
  case 3: {
    if (SweepState.active_mode) {
      // jump to animation start if active
      SweepState.pos = 0;
      return;
    }
  }
  }

  SweepState.pos++;

  if (SweepState.pos >= SweepState.loop_duration) {
    SweepState.pos = 0;
  }
}

// Spread the key's analog value outward from the centre LED
static void analogSpread(uint8_t button, RGB_Color_t color) {
  const uint8_t value = Analog_Get(button);
  // Ignore really low values
  if (value < 5) {
    return;
  }

  // Four segments across seven LEDs
  RGB_Color_t colors[4];
  const uint8_t segment_size = UINT8_MAX / 4;

  // Calculate segments
  for (uint8_t i = 0; i < 4; i++) {
    uint8_t brightness = 0;
    int16_t lower = i * segment_size;
    int16_t upper = lower + segment_size;

    if (value >= upper) {
      brightness = segment_size * 2;
    } else if (value > lower) {
      brightness = (value - lower) * 2;
    }

    colors[i] = (RGB_Color_t){.r = scale(color.r, brightness),
                              .g = scale(color.g, brightness),
                              .b = scale(color.b, brightness)};
  }

  // Place values into buffer
  const uint8_t led_start = button * CONTROLLER_RGB_LEDS_PER_KEY;

  // Start with centre LED
  bufferAdd(led_start + 3, colors[0]);

  bufferAdd(led_start + 2, colors[1]);
  bufferAdd(led_start + 4, colors[1]);

  bufferAdd(led_start + 1, colors[2]);
  bufferAdd(led_start + 5, colors[2]);

  bufferAdd(led_start, colors[3]);
  bufferAdd(led_start + 6, colors[3]);
}

// --- Callbacks ---

void CALLBACK_OnHardwareReady() {
  bufferInit();
  twinkleReset();
}

void CALLBACK_RGBCalculateNextFrame() {
  bufferInit();

  enum IdleNewState idle_state_update = idleUpdate();

  if (idle_state_update == IDLE) {
    sweepReset(false);
  }

  if (idle_state_update == ACTIVE) {
    twinkleReset();
    sweepReset(true);
  }

  // This paints the turntable solid during activity and with an animation
  // during idle and when switching state
  sweepUpdate();

  if (IdleState.is_idle) {
    // Run idle effects
    twinkleUpdate();
  } else {
    // Run analog spread effect across keys
    for (uint8_t i = 0; i < KEY_COUNT_PLAY; i++) {
      analogSpread(i, COLOR_KEYS_PLAY);
    }

    // Run analog spread effect across effect keys
    for (uint8_t i = 0; i < KEY_COUNT_EFFECT; i++) {
      analogSpread(i + KEY_INDEX_START_EFFECT, COLOR_KEYS_EFFECT);
    }
  }
}

void CALLBACK_OnRGBDrawFallback() {
  RGB_ClearAll(0);

  bufferRender();
}

void CALLBACK_OnRGBDrawUSB(USB_OutputReport_t *output) {
  RGB_ClearAll(0);

  for (uint8_t i = 0; i < KEY_COUNT_PLAY; i++) {
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
