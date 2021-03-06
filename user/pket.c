#include "pket.h"
#include "g/keymap_combo.h"

bool sw_win_active = false;
bool sw_app_active = false;
oneshot_state os_shft_state = os_up_unqueued;
oneshot_state os_ctrl_state = os_up_unqueued;
oneshot_state os_alt_state = os_up_unqueued;
oneshot_state os_cmd_state = os_up_unqueued;

static uint16_t non_combo_input_timer = 0;

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

// Oneshot functions
bool is_oneshot_cancel_key(uint16_t keycode) {
    switch (keycode) {
        case LOWER:
        case LOW_SPC:
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
        case LOW_SPC:
        case RAISE:
        case SYSTEM:
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

// Xcase functions
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

// Combo functions
bool get_combo_must_tap(uint16_t index, combo_t *combo) {
    switch(index) {
        case HCOM_DQUOT:
        case VCB_LN:
        case VCB_NH:
            return false;
    }
    return true;
}

uint16_t get_combo_term(uint16_t index, combo_t *combo) {
    char id;
    uint16_t term;
    switch (index) {
        case WF_LCBR:
        case FP_RCBR:
            id = timer_elapsed(non_combo_input_timer) > 350 ? '1' : '2';
            term = timer_elapsed(non_combo_input_timer) > 350 ? 25 : 5;
            break;

        case WP_CBR_PAIR:

        case RS_LPRN:
        case ST_RPRN:
        case RT_PRN_PAIR:

        case XC_COPY:
        case CV_PASTE:
        case LU_QUES_DOT:

        case LUY_SNAKE_SCREAM:
            id = '5';
            term = 25;
            break;

        case VCB_LN:
        case VCB_NH:
        case XV_CUT:
        case ZX_UNDO:
        case UY_QUOT:
        case EI_TAB:
        case NI_EQL:
            id = '3';
            term = 30;
            break;

        case NE_ESC:
            id = timer_elapsed(non_combo_input_timer) > 250 ? '4' : '7';
            term = timer_elapsed(non_combo_input_timer) > 250 ? 30 : 5;
            break;

        case SWE_AO:
        case SWE_AE:
        case SWE_OE:
            id = '8';
            term = 40;
            break;

        case LTGT_ARROW:
        case HCOM_DQUOT:

        case XCV_PASTE_SFT:
        case WFP_CBR_PAIR_IN:
        case RST_PRN_PAIR_IN:

        default:
            id = '6';
            term = 35;
            break;
    }
    update_combo_status(term, id);
    return term;
}

#define THUMB_EXTRA 75
#define INDEX_EXTRA 20
#define LONG_EXTRA 100
#define RING_EXTRA 80
#define PINKY_EXTRA 75

// Tapping term
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case ALT_S:
        case ALT_L:
        case HOME_R:
        case HOME_I:
            return TAPPING_TERM + RING_EXTRA;
        // case CTL_A: // same as HOME_A
        case CTL_SCLN:
        case HOME_A:
        case HOME_O:
            return TAPPING_TERM + PINKY_EXTRA;
        case GUI_D:
        case GUI_K:
        case HOME_S:
        case HOME_E:
            return TAPPING_TERM + LONG_EXTRA;
        case SFT_F:
        case SFT_J:
        case HOME_T:
        case HOME_N:
            return TAPPING_TERM + INDEX_EXTRA;
        case LOW_SPC:
            return TAPPING_TERM + THUMB_EXTRA;
        default:
            return TAPPING_TERM;
    }
}

bool get_tapping_force_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // case CTL_A:
        case ALT_S:
        case GUI_D:
        case SFT_F:
        case SFT_J:
        case GUI_K:
        case ALT_L:
        case CTL_SCLN:
        case HOME_A:
        case HOME_R:
        case HOME_S:
        case HOME_T:
        case HOME_N:
        case HOME_E:
        case HOME_I:
        case HOME_O:
            return true;
        default:
            return false;
    }
}

bool get_ignore_mod_tap_interrupt(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // case CTL_A:
        case ALT_S:
        case GUI_D:
        case SFT_F:
        case SFT_J:
        case GUI_K:
        case ALT_L:
        case CTL_SCLN:
        case HOME_A:
        case HOME_R:
        case HOME_S:
        case HOME_T:
        case HOME_N:
        case HOME_E:
        case HOME_I:
        case HOME_O:
            // Do not force the mod-tap key press to be handled as a modifier
            // if any other key was pressed while the mod-tap key is held down.
            return true;
        default:
            // Force the mod-tap key press to be handled as a modifier if any
            // other key was pressed while the mod-tap key is held down.
            return false;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    update_swapper(&sw_win_active, KC_LGUI, KC_TAB, SW_WIN, keycode, record);
    update_swapper(&sw_app_active, KC_LGUI, KC_GRV, SW_APP, keycode, record);

    update_oneshot(&os_shft_state, KC_LSFT, OS_LSFT, keycode, record);
    update_oneshot(&os_ctrl_state, KC_LCTL, OS_LCTL, keycode, record);
    update_oneshot(&os_alt_state, KC_LALT, OS_LALT, keycode, record);
    update_oneshot(&os_cmd_state, KC_LGUI, OS_LGUI, keycode, record);

    if (!process_case_modes(keycode, record)) {
        return false;
    }

    non_combo_input_timer = timer_read();

    static uint16_t single_quote_timer;
    static uint16_t double_quote_timer;

    if (record->event.pressed) {
        append_keylog(keycode);
    }

    switch (keycode) {
        // TODO could I merge these like the swe chars?
        case S_QUOTE:
            if (record->event.pressed) {
                single_quote_timer = timer_read();
                tap_code16(KC_QUOT);
            } else if (timer_elapsed(single_quote_timer) > TAPPING_TERM) {
                tap_code16(KC_QUOT);
                tap_code16(KC_LEFT);
            }
            return false;
        case D_QUOTE:
            if (record->event.pressed) {
                double_quote_timer = timer_read();
                tap_code16(KC_DQUO);
            } else if (timer_elapsed(double_quote_timer) > TAPPING_TERM) {
                tap_code16(KC_DQUO);
                tap_code16(KC_LEFT);
            }
            return false;
        // TODO could I merge this three like swe chars?
        case CURLYS:
            if (record->event.pressed) {
                SEND_STRING("{}"SS_TAP(X_LEFT));
            }
            return false;
        case BRCKETS:
            if (record->event.pressed) {
                SEND_STRING("[]"SS_TAP(X_LEFT));
            }
            return false;
        case PARENS:
            if (record->event.pressed) {
                SEND_STRING("()"SS_TAP(X_LEFT));
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
        // TODO these two are never used afaik
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
    }
    return true;
}

#ifdef OLED_ENABLE

void suspend_power_down_user(void) {
    oled_off();
}

#endif
