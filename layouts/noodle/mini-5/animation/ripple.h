#include "../defs.h"
#include "../lighting.h"
#include "../matrix.h"

typedef enum {
  ANIMATION_RIPPLE_TYPE_KEY = 0,
  ANIMATION_RIPPLE_TYPE_POINT = 1,
} AnimationRippleType_t;

typedef struct {
  AnimationRippleType_t type;
  uint8_t active;
  uint8_t origin;
  uint8_t step;
  uint8_t max_steps;
  uint8_t speed;
  RGB_Color_t color;
  TimerTick_t last_tick;
  uint16_t step_duration;
} AnimationRipple_t;

typedef struct {
  AnimationRipple_t ripples[ANIMATION_RIPPLE_MAX_RIPPLES];
  uint8_t count;
  uint8_t next_slot;
} AnimationRippleManager_t;

extern AnimationRippleManager_t ripple_manager;
extern TimerTick_t last_ripple_idle_tick;

void Ripple_Add(AnimationRippleType_t type, uint8_t origin, RGB_Color_t color,
                uint8_t speed);
void Ripple_AddRandom(RGB_Color_t color, uint8_t speed);
RGB_Color_t Ripple_ClampRGB(RGB_Color_t color);
void Ripple_Clear(void);
_Bool Ripple_HasActive(void);
void Ripple_Update(void);
void Ripple_UpdateIdle(void);
