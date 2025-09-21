#include "matrix.h"

// a matrix of the button LEDs. 255 is no LED.
const uint8_t Matrix_LEDMatrix[4][8] = {
    {255, 7, 6, 15, 14, 23, 22, 255},
    {255, 4, 5, 12, 13, 20, 21, 255},
    {3, 2, 11, 10, 19, 18, 27, 26},
    {0, 1, 8, 9, 16, 17, 24, 25},
};

// map each key to its 4 LEDs in the matrix
const uint8_t Matrix_LEDKeyRanges[7][4] = {
    {0, 1, 2, 3},     {4, 5, 6, 7},     {8, 9, 10, 11},   {12, 13, 14, 15},
    {16, 17, 18, 19}, {20, 21, 22, 23}, {24, 25, 26, 27},
};

// map each e key's LEDs
const uint8_t Matrix_LEDsE[] = {28, 29, 30, 31};

// map each TT LED
const uint8_t Matrix_LEDsTT[12] = {32, 33, 34, 35, 36, 37,
                                   38, 39, 40, 41, 42, 43};

KeyPosition_t Matrix_FindPosition(uint8_t led_index) {
  KeyPosition_t pos = {0, 0};
  for (uint8_t row = 0; row < 4; row++) {
    for (uint8_t col = 0; col < 8; col++) {
      if (Matrix_LEDMatrix[row][col] == led_index) {
        pos.row = row;
        pos.col = col;
        return pos;
      }
    }
  }
  return pos;
}

uint8_t Matrix_CalculateDistance(KeyPosition_t pos1, KeyPosition_t pos2) {
  int dx = (int)pos1.col - (int)pos2.col;
  int dy = (int)pos1.row - (int)pos2.row;
  uint8_t abs_dx = (dx < 0) ? -dx : dx;
  uint8_t abs_dy = (dy < 0) ? -dy : dy;
  return abs_dx + abs_dy;
}

void Matrix_GetKeyLEDs(uint8_t key, uint8_t *led_indices, uint8_t *count) {
  if (key >= 7) {
    *count = 0;
    return;
  }

  *count = 4;
  for (uint8_t i = 0; i < 4; i++) {
    led_indices[i] = Matrix_LEDKeyRanges[key][i];
  }
}
