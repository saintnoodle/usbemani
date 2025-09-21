#include "lighting.h"
#include "matrix.h"

// TODO: Be able to assign colours to specific and ranges of LEDs

RGB_Color_t lighting_buf[LIGHTING_BUF_SIZE];

void Lighting_Init(void) { Lighting_BufferClear(); }

void Lighting_BufferClear(void) {
  for (uint8_t i = 0; i < LIGHTING_BUF_SIZE; i++) {
    lighting_buf[i].r = 0;
    lighting_buf[i].g = 0;
    lighting_buf[i].b = 0;
  }
}

void Lighting_BufferAdd(uint8_t led_idx, RGB_Color_t color) {
  if (led_idx >= RGB_LEDS_PER_CHANNEL) {
    return;
  }

  uint16_t new_r = lighting_buf[led_idx].r + color.r;
  uint16_t new_g = lighting_buf[led_idx].g + color.g;
  uint16_t new_b = lighting_buf[led_idx].b + color.b;

  lighting_buf[led_idx].r = (new_r > 255) ? 255 : new_r;
  lighting_buf[led_idx].g = (new_g > 255) ? 255 : new_g;
  lighting_buf[led_idx].b = (new_b > 255) ? 255 : new_b;
}

void Lighting_BufferAddRange(uint8_t start_idx, uint8_t count,
                             RGB_Color_t color) {
  for (uint8_t i = 0; i < count; i++) {
    Lighting_BufferAdd(start_idx + i, color);
  }
}

void Lighting_Render(void) {
  RGB_ClearAll(LIGHTING_CHANNEL);

  for (uint8_t i = 0; i < LIGHTING_KEYS_COUNT; i++) {
    RGB_Set(LIGHTING_CHANNEL, i + LIGHTING_KEYS_START,
            Lighting_ClampRGBKey(lighting_buf[i]));
  }

  for (uint8_t i = 0; i < LIGHTING_E_COUNT; i++) {
    RGB_Set(LIGHTING_CHANNEL, i + LIGHTING_E_START,
            Lighting_ClampRGBKey(lighting_buf[i + LIGHTING_E_START]));
  }

  for (uint8_t i = 0; i < LIGHTING_TT_COUNT; i++) {
    RGB_Set(LIGHTING_CHANNEL, i + LIGHTING_TT_START,
            Lighting_ClampRGBTT(lighting_buf[i + LIGHTING_TT_START]));
  }
}

void Lighting_CalculateAnalogRGB(void) {
  for (uint8_t i = 0; i < 7; i++) {
    const RGB_Color_t color = Lighting_GetButtonColor(i + LIGHTING_KEYS_START);

    for (uint8_t j = 0; j < 4; j++) {
      uint8_t led_index = Matrix_LEDKeyRanges[i][j];
      Lighting_BufferAdd(led_index, color);
    }
  }

  for (uint8_t i = 0; i < 4; i++) {
    const RGB_Color_t color = Lighting_GetButtonColor(i + 7);
    Lighting_BufferAdd(i + LIGHTING_E_START, color);
  }
}

RGB_Color_t Lighting_GetButtonColor(uint8_t button) {
  RGB_Color_t color = {.r = 0, .g = 0, .b = 0};

  if (!Button_Get(button)) {
    return color;
  }

  const uint8_t uint_analog = (uint8_t)(Analog_Get(button));
  const uint8_t uint_analog_half = uint_analog / 3;
  const uint8_t value = (uint_analog_half < LIGHTING_CLAMP_VALUE)
                            ? uint_analog_half
                            : LIGHTING_CLAMP_VALUE;

  if (value < 8) {
    return color;
  }

  if (button >= 7) {
    // E1-E4
    if (button == 7) {
      color.r = (value * START_KEY_WEIGHT.r) >> 8;
      color.g = (value * START_KEY_WEIGHT.g) >> 8;
      color.b = (value * START_KEY_WEIGHT.b) >> 8;
    } else {
      color.r = (value * E_KEY_WEIGHT.r) >> 8;
      color.g = (value * E_KEY_WEIGHT.g) >> 8;
      color.b = (value * E_KEY_WEIGHT.b) >> 8;
    }
  } else if (button % 2 == 0) {
    // White keys
    const uint8_t value = (uint_analog_half < LIGHTING_MAX_KEY_BRIGHTNESS)
                              ? uint_analog_half
                              : LIGHTING_MAX_KEY_BRIGHTNESS;

    color.r = (value * WHITE_KEY_WEIGHT.r) >> 8;
    color.g = (value * WHITE_KEY_WEIGHT.g) >> 8;
    color.b = (value * WHITE_KEY_WEIGHT.b) >> 8;
  } else {
    // Black keys
    const uint8_t value = (uint_analog_half < LIGHTING_MAX_KEY_BRIGHTNESS)
                              ? uint_analog_half
                              : LIGHTING_MAX_KEY_BRIGHTNESS;

    color.r = (value * BLACK_KEY_WEIGHT.r) >> 8;
    color.g = (value * BLACK_KEY_WEIGHT.g) >> 8;
    color.b = (value * BLACK_KEY_WEIGHT.b) >> 8;
  }
  return color;
}

RGB_Color_t Lighting_RainbowCurrent(void) {
  const uint16_t tick = Timer_GetTicks();

  // Calculate hue based on time
  uint16_t hue = (tick >> 5) & 0x5FF;

  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  uint8_t segment = hue >> 8;
  uint8_t offset = hue & 0xFF;
  uint8_t inv_offset = 255 - offset;

  switch (segment) {
  case 0:
    r = 255;
    g = offset;
    b = 0;
    break;
  case 1:
    r = inv_offset;
    g = 255;
    b = 0;
    break;
  case 2:
    r = 0;
    g = 255;
    b = offset;
    break;
  case 3:
    r = 0;
    g = inv_offset;
    b = 255;
    break;
  case 4:
    r = offset;
    g = 0;
    b = 255;
    break;
  case 5:
    r = 255;
    g = 0;
    b = inv_offset;
    break;
  default:
    r = 255;
    g = 0;
    b = 0;
    break;
  }

  RGB_Color_t color = {.r = r, .g = g, .b = b};
  return color;
}

RGB_Color_t Lighting_ClampRGBKey(RGB_Color_t color) {
  LIGHTING_CLAMP_RGB(color, LIGHTING_MAX_KEY_BRIGHTNESS);
  return color;
}

RGB_Color_t Lighting_ClampRGBTT(RGB_Color_t color) {
  LIGHTING_CLAMP_RGB(color, LIGHTING_MAX_TT_BRIGHTNESS);
  return color;
}
