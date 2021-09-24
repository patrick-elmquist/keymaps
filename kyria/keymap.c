/* Copyright 2019 Thomas Baart <thomas@splitkb.com>
 * Copyright 2021 Patrick Elmquist
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "pket.h"
#include "g/keymap_combo.h"

#define LOW_ENT LT(_LOWER, KC_ENT)
#define LOW_SPC LT(_LOWER, KC_SPC)
#define RAI_ESC LT(_RAISE, KC_ESC)
#define RAI_REP LT(_RAISE, REPEAT)
#define RAI_ADP LT(_RAISE, ADAPT)
#define CTL_BSP LCTL_T(KC_BSPC)

bool sw_win_active = false;
oneshot_state os_shft_state = os_up_unqueued;
oneshot_state os_ctrl_state = os_up_unqueued;
oneshot_state os_alt_state = os_up_unqueued;
oneshot_state os_cmd_state = os_up_unqueued;

static uint16_t non_combo_input_timer = 0;

// TODO investigate if there are things in config.h that can be disabled
//      to regain some of that lost memory...
// TODO investigate long press of combos, like long press J+L for = would
//      result in ==, and longpressing M+, for $ could result in ${}<left>
// ISSUES
// - Callum mods don't work since the OS_RAISE key will reset the keys
//   when letting go. Need to add it as an exception.
// - Noticing some strain in using combos a lot, might want to move
//   keys I use a lot to a layer instead.
// - The combos for swe chars are not working out, I need something
//   better for that.
// - Starting to consider moving BSPC back to a thumb key, also to
//   reduce strain in wrist/hand.
// - Might want to remove the macros for single {()} but keep the
//   ones for pairs. By doing so, try shrinking the combos to be
//   next to each other.
// - The combo for CMD-Z sucks, remove it and add a key in SYSTEM
// - Add a CMD-W next to QUIT in SYSTEM
// - Use combos more for things that trigger multiple chars instead
//   of single, if pressing two fingers for one symbol, might as
//   well keep it on a layer instead.
// - Move QUOTES to a combo or at least away from the pinky.
// - Add keys for screenshots, especially the one to clipboard
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_QWERTY] = LAYOUT(
      KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                                        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_PIPE,
      CTL_BSP, CTL_A,   ALT_S,   GUI_D,   SFT_F,   KC_G,                                        KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
      KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    CAPS,    LOWER,   SNAKE,   SNK_SCM, KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_MINS,
                                 KC_LGUI, RAISE,   SYSTEM,  LOW_SPC, ADAPT,   LOW_ENT, OS_RAIS, RAI_ADP, ADAPT,   KC_LALT
    ),

    [_LOWER] = LAYOUT(
      _______, KC_EXLM, KC_AT,   KC_LCBR, KC_RCBR, KC_PIPE,                                     _______, _______, _______, _______, _______, KC_BSLS,
      _______, KC_HASH, KC_DLR,  KC_LPRN, KC_RPRN, KC_GRV,                                      KC_PLUS, KC_MINS, KC_SLSH, KC_ASTR, _______, KC_QUOT,
      _______, KC_PERC, KC_CIRC, KC_LBRC, KC_RBRC, KC_TILD, _______, _______, _______, _______, KC_AMPR, KC_EQL,  KC_COMM, KC_DOT,  KC_SLSH, KC_MINS,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

    [_RAISE] = LAYOUT(
      _______, KC_1, 	KC_2,    KC_3,    KC_4,    KC_5,                                        KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    _______,
      _______, OS_LCTL, OS_LALT, OS_LGUI, OS_LSFT, _______,                                     KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, QUOTES,
      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

    [_SYSTEM] = LAYOUT(
      _______, QUIT,    CLOSE,   SW_WIN,  RELOAD,  ITERM,                                       _______, _______, _______, _______, _______, _______,
      _______, KC_9,    KC_5,    KC_3,    KC_1,    ALFRED,                                      _______, KC_0,    KC_2,    KC_4,    KC_8,    _______,
      _______, _______, _______, _______, KC_7,    _______, _______, _______, _______, _______, _______, KC_6,    _______, _______, _______, _______,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

    [_ADJUST] = LAYOUT(
      _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                                       KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  _______,
      _______, _______, KC_MPRV, KC_MPLY, KC_MNXT, KC_VOLU,                                     _______, _______, _______, KC_F11,  KC_F12,  _______,
      _______, _______, _______, _______, KC_MUTE, KC_VOLD, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),
};

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

// Oneshot functions
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
uint16_t get_combo_term(uint16_t index, combo_t *combo) {
    char id;
    uint16_t term;
    switch (index) {
        case WE_LCBR:
        case ER_RCBR:
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
            id = timer_elapsed(non_combo_input_timer) > 300 ? '1' : '2';
            term = timer_elapsed(non_combo_input_timer) > 300 ? 30 : 5;
            break;

        case UIO_SNAKE_SCREAM:
            id = '3';
            term = 25;
            break;

        case HJ_ARROW:
        case LTGT_ARROW:
        case MCOM_DLR:
        case KL_TAB:
        case JK_ESC:
        default:
            id = '4';
            term = 35;
            break;
    }
    update_combo_status(term, id);
    return term;
}

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CTL_A:
        case ALT_S:
            return TAPPING_TERM + 50;
        case LOW_SPC:
            return TAPPING_TERM + 100;
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

bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CTL_A:
        case ALT_S:
        case GUI_D:
        case SFT_F:
            // Immediately select the hold action when another key is tapped.
            return true;
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

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_180;
}

static void render_qmk_logo(void) {
  static const char PROGMEM qmk_logo[] = {
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,
    0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,
    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,0};

  oled_write_P(qmk_logo, false);
}

static void render_status(void) {
    render_qmk_logo();

    render_empty_line();

    oled_write_P(PSTR(" Layer: "), false);
    switch (get_highest_layer(layer_state)) {
        case _QWERTY:
            oled_write_P(PSTR("Default\n"), false);
            break;
        case _LOWER:
            oled_write_P(PSTR("Lower\n"), false);
            break;
        case _RAISE:
            oled_write_P(PSTR("Raise\n"), false);
            break;
        case _SYSTEM:
            oled_write_P(PSTR("System\n"), false);
            break;
        case _ADJUST:
            oled_write_P(PSTR("Adjust\n"), false);
            break;
        default:
            oled_write_P(PSTR("Undefined\n"), false);
    }
    render_case_mode_status(get_xcase_delimiter(), caps_word_enabled());
    render_combo_status();
}

void oled_task_user(void) {
    if (is_keyboard_master()) {
        render_status();
    } else {
        render_mod_status_short();
    }
}

void suspend_power_down_user(void) {
    oled_off();
}
#endif

