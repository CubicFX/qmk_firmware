/* Copyright 2023 CubicFX (@CubicFX)
SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once

#define MATRIX_ROWS 6
#define MATRIX_COLS 7

#define ENCODER_BUTTON_PIN C15
#define ENCODER_ROW 1
#define ENCODER_COL 14

#define MUXES 2
#define MUX_PINS { F7, F6 }
#define MUX_SELECTOR_BITS 4
#define MUX_SELECTOR_PINS { D4, C6, D7, E6 }

#define MUX_CHANNELS ( 1 << MUX_SELECTOR_BITS )

#define EECONFIG_KB_DATA_SIZE 6

// #define DEBUG_ENABLE
#define DEBUG_MATRIX_SCAN_RATE

#define ADC_RESOLUTION      ADC_CFGR_RES_10BITS
#define ADC_RESOLUTION_MAX  1 << 10

#define CALIBRATION_RANGE 255


