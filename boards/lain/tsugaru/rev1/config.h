#pragma once

#define ANALOG_DRIVER magnetic
#define ANALOG_CHANNELS_AVAILABLE 16

#define BUTTON_DRIVER magnetic
#define BUTTONS_AVAILABLE 11

#define ENCODER_DRIVER direct
#define ENCODERS_AVAILABLE 2
#define ENCODER_PINS [0] = {24,25}, [1] = {26,27}

#define RGB_DRIVER pio
#define RGB_CHANNELS_AVAILABLE 1
#define RGB_PINS 22
#define RGB_PIO pio0

#define PSX_ACTIVE
#define PSX_DRIVER          pio
#define PSX_PIO             pio1
#define PSX_CMD_CLK_PINBASE 14
#define PSX_ATT_PIN         13
#define PSX_DAT_PIN         12
#define PSX_ACK_PIN         11

#define USB_CMD_REPORT_SIZE 64

/*** Chain Inclusion ***/
#include_next "config.h"
