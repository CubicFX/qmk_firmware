/* Copyright 2023 CubicFX (@CubicFX)
SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <stdint.h>

extern uint8_t lut[ADC_RESOLUTION_MAX];

const double lut_a;
const double lut_b;
const double lut_c;
const double lut_d;

uint16_t distance_to_adc(uint8_t distance);

uint8_t adc_to_distance(uint16_t adc);

void generate_lut(void);
