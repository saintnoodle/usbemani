#pragma once

#include "defs.h"

#define MAX_MAPPINGS 32

typedef enum {
  InputKey_1,
  InputKey_2,
  InputKey_3,
  InputKey_4,
  InputKey_5,
  InputKey_6,
  InputKey_7,
  InputKey_E1,
  InputKey_E2,
  InputKey_E3,
  InputKey_E4,
  // InputKey_TT_CCW,
  // InputKey_TT_CW,
} InputKey_t;

typedef enum { TRIGGER_SINGLE, TRIGGER_RANGE } KeyTriggerType_t;

typedef struct {
  InputKey_t start;
  InputKey_t end;
} KeyRange_t;

typedef struct {
  KeyTriggerType_t type;
  union {
    InputKey_t single;
    KeyRange_t range;
  } keys;
} KeyTrigger_t;

typedef void (*KeyHandler_t)(InputKey_t key);

typedef struct {
  KeyTrigger_t trigger;
  KeyHandler_t on_press;
  KeyHandler_t on_release;
} KeyMapping_t;

extern bool key_state[BUTTONS_AVAILABLE];
extern KeyMapping_t key_mappings[MAX_MAPPINGS];
extern uint8_t num_mappings;

bool Input_HasActiveKey(void);
bool Input_IsEncoderActive(void);
void Input_Update(void);

void Input_RegisterMapping(const KeyMapping_t *mapping);
void Input_UnregisterMapping(const KeyTrigger_t *trigger);

bool Input_IsKeyInRange(InputKey_t key, const KeyRange_t *range);
KeyRange_t Input_CreateRange(InputKey_t start, InputKey_t end);

_encoder_t Input_GetEncoder(void);