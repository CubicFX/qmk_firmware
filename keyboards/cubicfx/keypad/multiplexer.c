/* Copyright 2023 CubicFX (@CubicFX)
SPDX-License-Identifier: GPL-2.0-or-later */
#include "quantum.h"
#include "multiplexer.h"

const pin_t mux_pins[MUXES] = MUX_PINS;
const pin_t mux_selector_pins[MUX_SELECTOR_BITS] = MUX_SELECTOR_PINS;

void multiplexer_init(void) {
    for (uint8_t i = 0; i < MUX_SELECTOR_BITS; i++) {
        pin_t pin = mux_selector_pins[i];
        setPinOutput(pin);
    }
}

bool select_mux(uint8_t channel) {
    if (channel > MUX_CHANNELS) return 0;
    for (uint8_t i = 0; i < MUX_SELECTOR_BITS; i++) {
        writePin(mux_selector_pins[i], channel & (1 << i));
    }
    current_channel = channel;
    return 1;
}

const mux_t NC = {255,255}; // A coord with a Null pin (from JSON)
const mux_t mux_index[MUXES][MUX_CHANNELS] = {
    {{5,1},{4,1},{3,1},{5,2},{4,2},{3,2},{2,2},{2,1},{1,1},{1,2},{0,0},{1,0},{2,0},{3,0},{4,0},{5,0}}, // Mux 0
    {{3,4},{2,5},{1,5},{2,4},{1,4},{1,3},{2,3},{3,3},{4,3},{5,3},{5,4},{5,5},{5,6},{4,4},{4,5},{3,5}}  // Mux 1
};
