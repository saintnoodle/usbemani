#pragma once

#include "defs.h"

typedef struct {
  uint8_t row;
  uint8_t col;
} KeyPosition_t;

extern const uint8_t Matrix_LEDMatrix[4][8];
extern const uint8_t Matrix_LEDKeyRanges[7][4];

KeyPosition_t Matrix_FindPosition(uint8_t led_index);
uint8_t Matrix_CalculateDistance(KeyPosition_t pos1, KeyPosition_t pos2);
void Matrix_GetKeyLEDs(uint8_t key, uint8_t *led_indices, uint8_t *count);
