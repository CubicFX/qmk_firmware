/* Copyright 2023 RephlexZero (@RephlexZero)
SPDX-License-Identifier: GPL-2.0-or-later */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "moonboard.h"
#include "quantum.h"
#include "eeprom.h"
#include "scanfunctions.h"
#include "print.h"
#include "multiplexer.h"

analog_config g_config = {
    .mode = static_actuation,
    .actuation_point = 32,
    .press_sensitivity = 32,
    .release_sensitivity = 32,
    .press_hysteresis = 0,
    .release_hysteresis = 5
};

#ifdef BOOTMAGIC_ENABLE
void bootmagic_lite(void) {
    // TODO: Add RAW to keystruct?
}
#endif

#ifdef DEFERRED_EXEC_ENABLE

#    ifdef DEBUG_ENABLE
deferred_token debug_token;
bool debug_print(void) {
    static char rowBuffer[MATRIX_COLS * 6 + 1]; // +1 for '\0'
    static char temp[8];
    static uint8_t row = 0;
    rowBuffer[0] = '\0'; // Initialize the row buffer
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        analog_key_t *key = &keys[row][col];
        if(!key->raw) {
            snprintf(temp, sizeof(temp), " null   ");
        } else {
            snprintf(temp, sizeof(temp), "%5d  ", key->offset); // Include a space for separation
        }
        strcat(rowBuffer, temp);
    }
    strcat(rowBuffer, "\n");
    uprintf("%s", rowBuffer);
    row++;
    if(row >= MATRIX_ROWS) {
        row = 0;
        uprintf("\n");
        return false;
    }
    return true;
}

uint32_t debug_print_callback(uint32_t trigger_time, void *cb_arg) {
    debug_print();
    return 100;
}
#    endif

deferred_token idle_recalibrate_token;
bool           process_record_kb(uint16_t keycode, keyrecord_t *record) {
    extend_deferred_exec(idle_recalibrate_token, 300000);
    return true;
}

uint32_t idle_recalibrate_callback(uint32_t trigger_time, void *cb_arg) {
    get_sensor_offsets();
    return 10000;
}
#endif

void values_load(void) {
    eeconfig_read_kb_datablock(&g_config);
}

void values_save(void) {
    eeconfig_update_kb_datablock(&g_config);
}

void eeconfig_init_kb() {
    values_save();
}

void keyboard_post_init_kb(void) {
#ifdef DEFERRED_EXEC_ENABLE
#    ifdef DEBUG_ENABLE
    debug_token = defer_exec(1000, debug_print_callback, NULL);
#    endif
    idle_recalibrate_token = defer_exec(300000, idle_recalibrate_callback, NULL);
#endif
    values_load();
}

#ifdef VIA_ENABLE
void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    /* data = [ command_id, channel_id, value_id, value_data ] */
    uint8_t *command_id        = &(data[0]);
    uint8_t *channel_id        = &(data[1]);
    uint8_t *value_id_and_data = &(data[2]);

    if (*channel_id == id_custom_channel) {
        switch (*command_id) {
            case id_custom_set_value: {
                via_config_set_value(value_id_and_data);
                break;
            }
            case id_custom_get_value: {
                via_config_get_value(value_id_and_data);
                break;
            }
            case id_custom_save: {
                values_save();
                break;
            }
            default: {
                /* Unhandled message */
                *command_id = id_unhandled;
                break;
            }
        }
        return;
    }

    /* Return the unhandled state */
    *command_id = id_unhandled;

    /* DO NOT call raw_hid_send(data,length) here, let caller do this */
}

enum via_dynamic_actuation {
    id_mode = 1,
    id_actuation_point,
    id_press_sensitivity,
    id_release_sensitivity,
    id_press_hysteresis,
    id_release_hysteresis,
};

void via_config_set_value(uint8_t *data) {
    /* data = [ value_id, value_data ] */
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_mode:
            g_config.mode = *value_data;
            break;
        case id_actuation_point:
            g_config.actuation_point = *value_data * 255 / 40;
            break;
        case id_press_sensitivity:
            g_config.press_sensitivity = *value_data * 255 / 40;
            break;
        case id_release_sensitivity:
            g_config.release_sensitivity = *value_data * 255 / 40;
            break;
        case id_press_hysteresis:
            g_config.press_hysteresis = *value_data * 255 / 40;
            break;
        case id_release_hysteresis:
            g_config.release_hysteresis = *value_data * 255 / 40;
            break;
    }
}

void via_config_get_value(uint8_t *data) {
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_mode:
            *value_data = g_config.mode;
            break;
        case id_actuation_point:
            *value_data = g_config.actuation_point * 40 / 255;
            break;
        case id_press_sensitivity:
            *value_data = g_config.press_sensitivity * 40 / 255;
            break;
        case id_release_sensitivity:
            *value_data = g_config.release_sensitivity * 40 / 255;
            break;
        case id_press_hysteresis:
            *value_data = g_config.press_hysteresis * 40 / 255;
            break;
        case id_release_hysteresis:
            *value_data = g_config.release_hysteresis * 40 / 255;
            break;
    }
}
#endif

#ifdef OLED_ENABLE
bool oled_task_kb() {
    static const char image[] PROGMEM = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xf0, 0xfc, 0xff, 0xf8, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xfc, 0xbf, 0xbf, 0x9f, 0x9f, 0x1f, 0x1f, 0x1f, 0x5f, 0x5f, 0x5f, 0x1f, 0x1f, 0x1f, 0x7f, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x80, 0x87, 0xff, 0xff, 0xfe, 0xf8, 0xf0, 0xe0, 0x80, 0x00, 0x00, 0x00, 0x27, 0x27, 0x27, 0x27, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x0f, 0x0e, 0x08, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x80, 0xe0, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x37, 0xe0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf8, 0xf8, 0xf8, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf0, 0xf0, 0x30, 0xf0, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xf0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x7f, 0x7f, 0x3f, 0x9f, 0xbf, 0xbf, 0xbf, 0xbf, 0x9f, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x01, 0x00, 0xe0, 0xe0, 0x80, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0xf0, 0x00, 0x00, 0x00, 0xf0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x07, 0x0f, 0x0f, 0x0f, 0x1f, 0x1f, 0x1f, 0x1f, 0x3f, 0x3f, 0x3f, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x3f, 0x3f, 0x3f, 0x3f, 0x1f, 0x1f, 0x1f, 0x0f, 0x0f, 0x0f, 0x07, 0x03, 0x00, 0x00, 0x00, 0x80, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x3f, 0x3f, 0x1f, 0x80, 0xc0, 0xcf, 0xcf, 0xdf, 0x9f, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    oled_write_raw_P(image, sizeof(image));

    return false;
}
#endif
