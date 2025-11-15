#include "usbemani.h"

#define KEY_COUNT_PLAY 7
#define KEY_COUNT_EFFECT 4
#define KEY_COUNT_TOTAL (KEY_COUNT_PLAY + KEY_COUNT_EFFECT)

#define KEY_INDEX_START_PLAY 0
#define KEY_INDEX_START_EFFECT KEY_COUNT_PLAY

#define LED_COUNT_PLAY (KEY_COUNT_PLAY * CONTROLLER_RGB_LEDS_PER_KEY)
#define LED_COUNT_EFFECT (KEY_COUNT_EFFECT * CONTROLLER_RGB_LEDS_PER_KEY)
#define LED_COUNT_KEYS (KEY_COUNT_TOTAL * CONTROLLER_RGB_LEDS_PER_KEY)

#define LED_INDEX_START_PLAY 0
#define LED_INDEX_START_EFFECT LED_COUNT_PLAY
#define LED_INDEX_START_TT (LED_INDEX_START_EFFECT + LED_COUNT_EFFECT)

#define LED_BUFFER_SIZE                                                        \
  (LED_COUNT_PLAY + LED_COUNT_EFFECT + CONTROLLER_RGB_LEDS_TURNTABLE)

#define IDLE_TIMEOUT_SECS 20

#define TWINKLE_PROBABILITY 13
#define TWINKLE_MIN_LIFE 600
#define TWINKLE_MAX_LIFE 960

enum IdleNewState { ACTIVE, IDLE, UNCHANGED };

typedef struct {
  _Bool is_idle;
  TimerTick_t last_active;
  const uint16_t timeout;
} IdleState_t;

typedef struct {
  RGB_Color_t color;
  uint16_t life;
  uint16_t max_life;
  TimerTick_t last_tick;
} TwinkleState_t;

enum SpinDirection_t { CW, CCW };

typedef struct {
  bool active_mode;
  uint16_t pos;
  const uint16_t loop_duration;
  const uint16_t led_start_pos;
  const enum SpinDirection_t direction;
} SweepState_t;
