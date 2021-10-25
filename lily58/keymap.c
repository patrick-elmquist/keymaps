#include QMK_KEYBOARD_H

#include "keymap.h"
#include "g/keymap_combo.h"

bool sw_win_active = false;
oneshot_state os_shft_state = os_up_unqueued;
oneshot_state os_ctrl_state = os_up_unqueued;
oneshot_state os_alt_state = os_up_unqueued;
oneshot_state os_cmd_state = os_up_unqueued;

static uint16_t non_combo_input_timer = 0;

// TODO move enter and change thumb shift to one shot and perhaps toggle Word cap on double tap
// TODO update the combo term since io and er seems to misfire
// TODO can I solve the issues with CMD (SFT) + N by having OSM CMD and SFT on SPC and ENT
// TODO would it be nice to move enter to a layer or combo and use ENT as OS SFT instead of having to press it
//      perhaps a follow up to the above could be to toggle CAPS Word if pressing the ENT OSM twice
// TODO when adding a number layer, make dubble tapping it toggle NUMWORD
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

uint16_t get_combo_term(uint16_t index, combo_t *combo) {
    char id;
    uint16_t term;
    switch (index) {
        case WE_LCBR:
        case ER_RCBR:
            id = timer_elapsed(non_combo_input_timer) > 350 ? '1' : '2';
            term = timer_elapsed(non_combo_input_timer) > 350 ? 25 : 5;
            break;

        case WR_CBR_PAIR:

        case SD_LPRN:
        case DF_RPRN:
        case SF_PRN_PAIR:

        case XC_COPY:
        case CV_PASTE:
        case XV_CUT:
        case ZX_UNDO:

        case UI_QUES_DOT:
        case IO_NOT_EQL:

        case XCV_PASTE_SFT:
        case WER_CBR_PAIR_IN:
        case SDF_PRN_PAIR_IN:
            id = timer_elapsed(non_combo_input_timer) > 300 ? '3' : '4';
            term = timer_elapsed(non_combo_input_timer) > 300 ? 30 : 5;
            break;

        case UIO_SNAKE_SCREAM:
            id = '5';
            term = 25;
            break;

        case HJ_ARROW:
        case LTGT_ARROW:
        case MCOM_DLR:
        case KL_TAB:
        case JK_ESC:
        default:
            id = '6';
            term = 35;
            break;
    }
    update_combo_status(term, id);
    return term;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

bool is_oneshot_cancel_key(uint16_t keycode) {
    switch (keycode) {
        case LOWER:
        case RAISE:
        case SYSTEM:
            return true;
        default:
            return false;
    }
}

bool is_oneshot_ignored_key(uint16_t keycode) {
    switch (keycode) {
        case LOWER:
        case RAISE:
        case KC_LSFT:
        case OS_LSFT:
        case OS_LCTL:
        case OS_LALT:
        case OS_LGUI:
            return true;
        default:
            return false;
    }
}

bool terminate_case_modes(uint16_t keycode, const keyrecord_t *record) {
    switch (keycode) {
        // Keycodes to ignore (don't disable caps word)
        case KC_A ... KC_Z:
        case KC_1 ... KC_0:
        case KC_MINS:
        case KC_UNDS:
        case KC_BSPC:
        case CAPS:
        case SNK_SCM:
        case SNAKE:
            // If mod chording disable the mods
            if (record->event.pressed && (get_mods() != 0)) {
                return true;
            }
            break;
        default:
            if (record->event.pressed) {
                return true;
            }
            break;
    }
    return false;
}

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case ALT_S:
            return TAPPING_TERM + 100;
        case LOW_SPC:
            return TAPPING_TERM + 100;
        case CTL_A:
            return TAPPING_TERM + 75;
        case GUI_D:
        case SFT_F:
        default:
            return TAPPING_TERM;
    }
}

bool get_tapping_force_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CTL_A:
        case ALT_S:
        case GUI_D:
        case SFT_F:
            return true;
        default:
            return false;
    }
}

bool get_ignore_mod_tap_interrupt(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CTL_A:
        case ALT_S:
        case GUI_D:
        case SFT_F:
            // Do not force the mod-tap key press to be handled as a modifier
            // if any other key was pressed while the mod-tap key is held down.
            return true;
        default:
            // Force the mod-tap key press to be handled as a modifier if any
            // other key was pressed while the mod-tap key is held down.
            return false;
    }
}

// TODO this is effectively disabled now, if this feels better,
//      remove the function and the entry in config instead.
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CTL_A:
        case ALT_S:
        case GUI_D:
        case SFT_F:
            // Immediately select the hold action when another key is tapped.
            return false;
        default:
            // Do not select the hold action when another key is tapped.
            return false;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    update_swapper(&sw_win_active, KC_LGUI, KC_TAB, SW_WIN, keycode, record);

    update_oneshot(&os_shft_state, KC_LSFT, OS_LSFT, keycode, record);
    update_oneshot(&os_ctrl_state, KC_LCTL, OS_LCTL, keycode, record);
    update_oneshot(&os_alt_state, KC_LALT, OS_LALT, keycode, record);
    update_oneshot(&os_cmd_state, KC_LGUI, OS_LGUI, keycode, record);

    if (!process_case_modes(keycode, record)) {
        return false;
    }

    non_combo_input_timer = timer_read();

    if (record->event.pressed) {
        append_keylog(keycode);
    }

    uint16_t last_keycode = last_key();
    if (record->event.pressed && keycode != REPEAT) {
        register_key_to_repeat(keycode);
    }

    static uint16_t os_sft_raise_timer;
    static uint16_t last_combo;
    switch (keycode) {
        case QUOTES:
            if (record->event.pressed) {
                last_combo = keycode;
                SEND_STRING("\"\""SS_TAP(X_LEFT));
            }
            return false;
        case REPEAT:
            update_repeat_key(record);
            return false;
        case CURLYS:
            if (record->event.pressed) {
                last_combo = keycode;
                SEND_STRING("{}"SS_TAP(X_LEFT));
            }
            return false;
        case PARENS:
            if (record->event.pressed) {
                last_combo = keycode;
                SEND_STRING("()"SS_TAP(X_LEFT));
            }
            return false;
        case CTL_BSP:
            if (record->event.pressed) {
                switch (last_keycode) {
                    case CURLYS:
                        tap_code16(KC_END);
                        tap_code16(KC_BSPC);
                        tap_code16(KC_BSPC);
                        return false;
                    case PARENS:
                    case QUOTES:
                        tap_code16(KC_END);
                        tap_code16(KC_BSPC);
                        tap_code16(KC_BSPC);
                        return false;
                }
            }
            return true;
        case ADAPT:
            if (record->event.pressed) {
                switch (last_keycode) {
                    case CURLYS:
                        SEND_STRING("->"SS_TAP(X_ENTER));
                        return false;
                    case PARENS:
                        SEND_STRING("\"\""SS_TAP(X_LEFT));
                        return false;
                }
                switch (last_combo) {
                    case QUOTES:
                        last_combo = KC_NO;
                        SEND_STRING(":$");
                        return false;
                    case CURLYS:
                        last_combo = KC_NO;
                        SEND_STRING("-> ");
                        return false;
                    case PARENS:
                        last_combo = KC_NO;
                        tap_code16(KC_END);
                        tap_code16(KC_SPACE);
                        return false;
                }

            }
            return false;
        case SE_AO:
            if (record->event.pressed) {
                tap_code16(A(KC_A));
            }
            return false;
        case SE_AE:
        case SE_OE:
            if (record->event.pressed) {
                uint8_t mod_state = get_mods();
                if (mod_state & MOD_MASK_SHIFT) {
                    del_mods(MOD_MASK_SHIFT);
                }
                tap_code16(A(KC_U));
                set_mods(mod_state);
                tap_code16(keycode == SE_AE ? KC_A : KC_O);
            }
            return false;
        case CAPS:
            if (record->event.pressed) {
                toggle_caps_word();
            }
            return false;
        case SNAKE:
        case CAMEL:
            if (record->event.pressed) {
                if (get_xcase_delimiter() != KC_NO) {
                    disable_xcase();
                } else {
                    enable_xcase_with(keycode == SNAKE ? KC_UNDS : OSM(MOD_LSFT));
                }
            }
            return false;
        case SNK_SCM:
            if (record->event.pressed) {
                if (get_xcase_delimiter() != KC_NO) {
                    disable_xcase();
                    disable_caps_word();
                } else {
                    enable_caps_word();
                    enable_xcase_with(KC_UNDS);
                }
            }
            return false;
        case OS_RAIS:
            if (record->event.pressed) {
                os_sft_raise_timer = timer_read();
                layer_on(_RAISE);
            } else {
                layer_off(_RAISE);
                if (timer_elapsed(os_sft_raise_timer) < TAPPING_TERM + 50) {
                    set_oneshot_mods(MOD_LSFT);
                }
            }
            return false;
    }
    return true;
}

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

void suspend_power_down_user(void) {
    oled_off();
}

