#include "pket.h"

// TODO move enter and change thumb shift to one shot and perhaps toggle Word cap on double tap
// TODO update the combo term since io and er seems to misfire
// TODO can I solve the issues with CMD (SFT) + N by having OSM CMD and SFT on SPC and ENT
// TODO make double tapping the number layer toggle NUMWORD
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_QWERTY] = LAYOUT_wrapper( \
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        _____________________QWERTY_L1______________________,                    _____________________QWERTY_R1______________________,
        _____________________QWERTY_L2______________________,                    _____________________QWERTY_R2______________________,
        _____________________QWERTY_L3______________________, KC_LBRC,  KC_RBRC, _____________________QWERTY_R3______________________,
                                      XXXXXXX, RAISE, SYSTEM, LOW_SPC,  KC_ENTER, RAISE, LOWER,  XXXXXXX
        ),

    [_LOWER] = LAYOUT_wrapper( \
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        _____________________LOWER__L1______________________,                    _____________________LOWER__R1______________________,
        _____________________LOWER__L2______________________,                    _____________________LOWER__R2______________________,
        _____________________LOWER__L3______________________, _______,  _______, _____________________LOWER__R3______________________,
                                   _______, _______, _______, _______,  _______, _______, _______, _______
        ),

    [_RAISE] = LAYOUT_wrapper( \
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        _____________________RAISE__L1______________________,                    _____________________RAISE__R1______________________,
        _____________________RAISE__L2______________________,                    _____________________RAISE__R2______________________,
        _____________________RAISE__L3______________________, _______,  _______, _____________________RAISE__R3______________________,
                                   _______, _______, _______, _______,  _______, _______, _______, _______
        ),

    [_ADJUST] = LAYOUT_wrapper( \
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        _____________________ADJUST_L1______________________,                    _____________________ADJUST_R1______________________,
        _____________________ADJUST_L2______________________,                    _____________________ADJUST_R2______________________,
        _____________________ADJUST_L3______________________, _______,  _______, _____________________ADJUST_R3______________________,
                                   _______, _______, _______, _______,  _______, _______, _______, _______
        ),

    [_SYSTEM] = LAYOUT_wrapper( \
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        _____________________SYSTEM_L1______________________,                    _____________________SYSTEM_R1______________________,
        _____________________SYSTEM_L2______________________,                    _____________________SYSTEM_R2______________________,
        _____________________SYSTEM_L3______________________, _______,  _______, _____________________SYSTEM_R3______________________,
                                   _______, _______, _______, _______,  _______, _______, _______, _______
        ),
};

#ifdef OLED_ENABLE

void oled_task_user(void) {
    if (is_keyboard_master()) {
        render_empty_line();
        render_default_layer_state();
        render_empty_line();
        render_case_mode_status(get_xcase_delimiter(), caps_word_enabled());
        render_empty_line();
        render_keylogger_status();
        render_empty_line();
        render_combo_status();
    } else {
        render_empty_line();
        render_modifier_status();
    }
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (is_keyboard_master()) {
        return OLED_ROTATION_270;
    } else {
        return OLED_ROTATION_180;
    }
}

#endif
