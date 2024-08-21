/* Copyright 2023 CubicFX (@CubicFX)
SPDX-License-Identifier: GPL-2.0-or-later */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "quantum.h"
#include "analog.h"
#include "lut.h"
#include "multiplexer.h"
#include "scanfunctions.h"

// External definitions
extern const mux_t mux_index[MUXES][MUX_CHANNELS];



analog_key_t    keys[MATRIX_ROWS][MATRIX_COLS] = {0};
static uint16_t pressedAdcValue                = 0;
static uint16_t restAdcValue                   = 0;

void matrix_init_custom(void) {
    // generate_lut();
    pressedAdcValue = distance_to_adc(255);
    restAdcValue    = distance_to_adc(0);
    multiplexer_init();
    wait_ms(100);
    get_sensor_offsets();
}

matrix_row_t previous_matrix[MATRIX_ROWS];

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    memcpy(previous_matrix, current_matrix, sizeof(previous_matrix));

    for (uint8_t channel = 0; channel < MUX_CHANNELS; ++channel) {
        uint8_t channel_greycoded = (channel >> 1) ^ channel;
        select_mux(channel_greycoded);

        for (uint8_t mux = 0; mux < MUXES; ++mux) {
            const mux_t *mux_idx = &mux_index[mux][channel_greycoded];

            if (mux_idx->row == 255 && mux_idx->col == 255) continue; // NC mux pin

            analog_key_t *key = &keys[mux_idx->row][mux_idx->col];
            key->raw          = analogReadPin(mux_pins[mux]);
            key->value        = adc_to_distance(key->raw + key->offset);

            switch (g_config.mode) {
                case dynamic_actuation:
                    matrix_read_cols_dynamic_actuation(&current_matrix[mux_idx->row], mux_idx->col, key);
                    break;
                case continuous_dynamic_actuation:
                    matrix_read_cols_continuous_dynamic_actuation(&current_matrix[mux_idx->row], mux_idx->col, key);
                    break;
                case static_actuation:
                    matrix_read_cols_static_actuation(&current_matrix[mux_idx->row], mux_idx->col, key);
                    break;
                case flashing:
                default:
                    bootloader_jump();
                    break;
            }
        }
    }
    return memcmp(previous_matrix, current_matrix, sizeof(previous_matrix)) != 0;
}
