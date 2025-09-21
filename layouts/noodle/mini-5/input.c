#include "input.h"
#include "lighting.h"
#include "matrix.h"

bool key_state[BUTTONS_AVAILABLE] = {0};
KeyMapping_t key_mappings[MAX_MAPPINGS];
uint8_t num_mappings = 0;

bool Input_HasActiveKey(void) {
  for (uint8_t i = 0; i < BUTTONS_AVAILABLE; i++) {
    if (Button_Get(i)) {
      return true;
    }
  }
  return false;
}

bool Input_IsEncoderActive(void) {

  if (Encoder_Direction(0)) {
    return true;
  }

  return false;
}

bool Input_IsKeyInRange(InputKey_t key, const KeyRange_t *range) {
  return (key >= range->start && key <= range->end);
}

KeyRange_t Input_CreateRange(InputKey_t start, InputKey_t end) {
  KeyRange_t range = {.start = start, .end = end};
  return range;
}

void Input_RegisterMapping(const KeyMapping_t *mapping) {
  if (num_mappings < MAX_MAPPINGS) {
    key_mappings[num_mappings] = *mapping;
    num_mappings++;
  }
}

void Input_UnregisterMapping(const KeyTrigger_t *trigger) {
  for (uint8_t i = 0; i < num_mappings; i++) {
    if (memcmp(&key_mappings[i].trigger, trigger, sizeof(KeyTrigger_t)) == 0) {

      if (i < num_mappings - 1) {
        memmove(&key_mappings[i], &key_mappings[i + 1],
                (num_mappings - i - 1) * sizeof(KeyMapping_t));
      }

      num_mappings--;
      break;
    }
  }
}

static void ProcessKeyMappings(InputKey_t key, bool is_press) {
  for (uint8_t i = 0; i < num_mappings; i++) {
    const KeyMapping_t *mapping = &key_mappings[i];
    bool should_trigger = false;

    switch (mapping->trigger.type) {
    case TRIGGER_SINGLE:
      should_trigger = (key == mapping->trigger.keys.single);
      break;

    case TRIGGER_RANGE:
      should_trigger = Input_IsKeyInRange(key, &mapping->trigger.keys.range);
      break;
    }

    if (should_trigger) {
      if (is_press && mapping->on_press) {
        mapping->on_press(key);
      } else if (!is_press && mapping->on_release) {
        mapping->on_release(key);
      }
    }
  }
}

void Input_Update(void) {
  for (uint8_t i = 0; i < BUTTONS_AVAILABLE; i++) {
    const bool current_key_state = Button_Get(i);

    if (key_state[i] == current_key_state) {
      continue;
    }

    if (current_key_state) {
      ProcessKeyMappings(i, true);
    } else {
      ProcessKeyMappings(i, false);
    }

    key_state[i] = current_key_state;
  }

  // const _encoder_t encoder = Input_GetEncoder();
  // if (encoder.direction.current == ENCODER_CCW) {
  //   ProcessKeyMappings(InputKey_TT_CCW, true);
  // } else if (encoder.direction.current == ENCODER_CW) {
  //   ProcessKeyMappings(InputKey_TT_CW, true);
  // }
}

// a simple wrapper around the global encoder state
_encoder_t Input_GetEncoder(void) { return _encoder[0]; }