#pragma once

/*** Analogs ***/
#define ANALOG_CALIBRATION_UP      2340, 2302, 2368, 2369, 2389, 2291, 2320, 2401, 2305, 2288, 2322
#define ANALOG_CALIBRATION_DOWN    4011, 3920, 4035, 4043, 4033, 3938, 3843, 4054, 4053, 3998, 3999

/*** Buttons ***/
#define BUTTON_UP_THRESHOLD        (255 * 0.05) // Consider a button released at 5%
#define BUTTON_DOWN_THRESHOLD      (255 * 0.10) // Consider a button pressed at 10%

/*** Encoders ***/
#define ENCODER_CHANNELS_REVERSED
#define ENCODER_CHANNELS                0, 1
#define ENCODER_FREQUENCY               16000
#define ENCODER_SAMPLES_UNTIL_VALID     255
#define ENCODER_DIRECTION_THRESHOLD     2
#define ENCODER_DIRECTION_HOLD_TIMEOUT  200

/*** RGB ***/
#define RGB_CHANNELS          0
#define RGB_FRAMERATE_TARGET  120

/*** USB ***/
#define USB_ANALOGS_ACTIVE      11
#define USB_BUTTONS_ACTIVE      13
#define USB_ENCODER_AXES_ACTIVE 2
#define USB_ENCODER_AXES        USB_Axis_X, USB_Axis_Y
#define USB_GENERAL_AXES_ACTIVE 2
#define USB_GENERAL_AXES        USB_Axis_X, USB_Axis_Y
#define USB_RGB_ACTIVE          12

/*** Button Combinations ***/
#define BUTTON_COMBO_ARCH_RESET     0, 1
#define BUTTON_COMBO_KONAMI_CLOUD   7, 8

/*** Quirks ***/
#define QUIRK_IIDX_LR2_BUTTON_COMBO 1

/*** Additional Includes ***/
#include "usb_labels.h"
