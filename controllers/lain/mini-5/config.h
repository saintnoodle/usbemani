#pragma once

/*** Controller Type ***/
#define CONTROLLER_TYPE iidx

/*** Analogs ***/
#define ANALOG_CHANNELS_ACTIVE    11
#define ANALOG_MAGNETIC_PIN_MASK  0x1E // GPIO pins 1-4 for multiplexing
#define ANALOG_MAGNETIC_ADC_INPUT 2    // GPIO pin 28 for ADC input

/*** Buttons ***/
#define BUTTONS_ACTIVE  11

/*** Encoders ***/
#define ENCODERS_ACTIVE 2
#define ENCODER_PPR     72

/*** RGB ***/
#define RGB_CHANNELS_ACTIVE   1
#define RGB_LED_TYPE          WS2812
#define RGB_LEDS_PER_CHANNEL  (7 * CONTROLLER_RGB_LEDS_PER_KEY) + (4 * 1) + (CONTROLLER_RGB_LEDS_TURNTABLE)

/*** Chain Inclusion ***/
#include_next "config.h"
