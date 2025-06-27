#pragma once

/*** Buttons ***/
#define BUTTON_UP_THRESHOLD      (255 * 0.05) //  5%
#define BUTTON_DOWN_THRESHOLD    (255 * 0.10) // 10%

#define BUTTON_CALIBRATION_UP    1905, 1863, 1942, 1932, 2086, 2029, 1990, 1762, 1890, 1915, 2102
#define BUTTON_CALIBRATION_DOWN  1209, 1172, 1186, 1197, 1285, 1281, 1217, 1142, 1156, 1173, 1242

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
