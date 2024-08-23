/* Copyright 2023 CubicFX (@CubicFX)
SPDX-License-Identifier: GPL-2.0-or-later */
#include <math.h>
#include "scanfunctions.h"
#include "util.h"

 // const double lut_a = 16654600.6755; // Lekker
 // const double lut_b = -0.00955994866577;
 // const double lut_c = -1278.75103145;
 // const double lut_d = 16652478.4163;

/* Equation parameters for the sensor-magnet linearity mapping */
// const double lut_a = 0.200347177016; // Latenpow
// const double lut_b = 0.00955994866154;
// const double lut_c = 6.01110636956;
// const double lut_d = 1966.74076381;

//  const double lut_a = -0.0245757915451; // Cubic Lekker L45
//  const double lut_b = 0.0110907899281;
//  const double lut_c = 5.9088903529;
//  const double lut_d = 347.913976103;

 const double lut_a = 0.879838688261; // Cubic Gateron Double-Rail Magnetic - Aurora
 const double lut_b = 0.00663382091914;
 const double lut_c = 2.9113558737;
 const double lut_d = 347.719500597;

uint16_t distance_to_adc(uint8_t distance) {
    double intermediate = lut_a * exp(lut_b * distance + lut_c) + lut_d;
    uint16_t adc = (uint16_t) MAX(0, MIN(intermediate, 4095));
    return adc;
}

uint8_t adc_to_distance(uint16_t adc) {
    double check = (adc - lut_d) / lut_a;
    if (check <= 0) {
        return 0;
    }
    double intermediate = (log(check) - lut_c) / lut_b;
    uint8_t distance = (uint8_t) MAX(0, MIN(intermediate, 255));
    return distance;
}

// uint8_t lut[ADC_RESOLUTION_MAX] = {0};

// void generate_lut(void) {
//     for (uint16_t i = 0; i < ADC_RESOLUTION_MAX; i++) {
//         lut[i] = adc_to_distance(i);
//     }
// }
