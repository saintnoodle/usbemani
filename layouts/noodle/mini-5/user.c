#include "animation/ripple.h"
#include "animation/scan.h"
#include "defs.h"
#include "idle.h"
#include "input.h"
#include "lighting.h"
#include "matrix.h"
#include "usbemani.h"

// *** idle callbacks ***
void on_idle(void) {
#if defined(ANIMATION_RIPPLE_IDLE_ENABLED)
  Ripple_Clear();
#endif
}

void calculate_idle_frame(void) {
#if defined(ANIMATION_RIPPLE_IDLE_ENABLED)
  Ripple_UpdateIdle();
#endif
}

const Idle_Callbacks_t idle_callbacks = {
    .on_idle = on_idle,
    .on_calculate_frame = calculate_idle_frame,
    .on_reset = on_idle,
};

// *** system callbacks ***

void CALLBACK_RGBCalculateNextFrame() {
  Lighting_BufferClear();
  Lighting_CalculateAnalogRGB();

  Input_Update();
  Idle_Update(idle_callbacks);

#if defined(ANIMATION_RIPPLE_ENABLED)
  Ripple_Update();
#endif
}

void CALLBACK_OnRGBDrawFallback() { Lighting_Render(); }

void CALLBACK_OnRGBDrawUSB(USB_OutputReport_t *output) {
  RGB_ClearAll(0);

  for (int i = 0; i < 7; i++) {
    RGB_SetRange(0, (i * CONTROLLER_RGB_LEDS_PER_KEY),
                 CONTROLLER_RGB_LEDS_PER_KEY, output->rgb[i]);
  }

  for (uint8_t i = 0; i < 4; i++) {
    RGB_Set(0, LIGHTING_E_START + i, output->rgb[7 + i]);
  }

  RGB_SetRange(0, LIGHTING_TT_START, CONTROLLER_RGB_LEDS_TURNTABLE,
               output->rgb[7 + 4]);
}

// *** key mappings ***

// *** key ranges ***
const KeyRange_t main_keys = {
    .start = InputKey_1,
    .end = InputKey_7,
};

const KeyRange_t e_keys = {
    .start = InputKey_E1,
    .end = InputKey_E4,
};

// *** key callbacks ***

void main_keys_on_press(InputKey_t key) {
#if defined(ANIMATION_RIPPLE_ENABLED)
  RGB_Color_t color = Lighting_GetButtonColor(key);
  Ripple_Add(ANIMATION_RIPPLE_TYPE_KEY, key, color, ANIMATION_RIPPLE_SPEED);
#endif
}

// *** bindings ***

const KeyMapping_t main_keys_mapping = {
    .trigger =
        {
            .type = TRIGGER_RANGE,
            .keys =
                {
                    .range = main_keys,
                },
        },
    .on_press = main_keys_on_press,
    .on_release = NULL,
};

/*** init ***/

void CALLBACK_OnHardwareReady() {
  Lighting_Init();

  // register all bindings
  Input_RegisterMapping(&main_keys_mapping);
}