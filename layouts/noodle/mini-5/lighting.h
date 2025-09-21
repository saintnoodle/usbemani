#pragma once

#include "defs.h"

#define LIGHTING_CHANNEL 0

// LED counts
#define LIGHTING_E_COUNT 4
#define LIGHTING_TT_COUNT CONTROLLER_RGB_LEDS_TURNTABLE
#define LIGHTING_KEYS_COUNT (7 * CONTROLLER_RGB_LEDS_PER_KEY)

// LED starting indices
#define LIGHTING_KEYS_START 0
#define LIGHTING_E_START (LIGHTING_KEYS_START + LIGHTING_KEYS_COUNT)
#define LIGHTING_TT_START (LIGHTING_E_START + LIGHTING_E_COUNT)

#define LIGHTING_BUF_SIZE (LIGHTING_TT_START + LIGHTING_TT_COUNT)

extern RGB_Color_t lighting_buf[LIGHTING_BUF_SIZE];

#define LIGHTING_CLAMP_RGB(color, max_brightness)                              \
  do {                                                                         \
    if ((color).r > (max_brightness)) {                                        \
      (color).r = (max_brightness);                                            \
    }                                                                          \
    if ((color).g > (max_brightness)) {                                        \
      (color).g = (max_brightness);                                            \
    }                                                                          \
    if ((color).b > (max_brightness)) {                                        \
      (color).b = (max_brightness);                                            \
    }                                                                          \
  } while (0)

void Lighting_BufferClear(void);
void Lighting_BufferAdd(uint8_t led_idx, RGB_Color_t color);
void Lighting_BufferAddRange(uint8_t start_idx, uint8_t count,
                             RGB_Color_t color);
void Lighting_Init(void);
void Lighting_Render(void);
void Lighting_CalculateAnalogRGB(void);

RGB_Color_t Lighting_GetButtonColor(uint8_t button);
RGB_Color_t Lighting_RainbowCurrent(void);
RGB_Color_t Lighting_ClampRGBKey(RGB_Color_t color);
RGB_Color_t Lighting_ClampRGBTT(RGB_Color_t color);
