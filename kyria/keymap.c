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

#include "pket.h"

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

    [_QWERTY] = LAYOUT_wrapper(
      _____________________QWERTY_L1______________________,                                     _____________________QWERTY_R1______________________,
      _____________________QWERTY_L2______________________,                                     _____________________QWERTY_R2______________________,
      _____________________QWERTY_L3______________________, CAPS,    LOWER,   SNAKE,   SNK_SCM, _____________________QWERTY_R3______________________,
                                 KC_LGUI, _________MOD_LEFT________, ADAPT,   LOW_ENT, _________MOD_RIGHT_______, KC_LALT
    ),

    [_LOWER] = LAYOUT_wrapper(
      _____________________LOWER__L1______________________,                                     _____________________LOWER__R1______________________,
      _____________________LOWER__L2______________________,                                     _____________________LOWER__R2______________________,
      _____________________LOWER__L3______________________, _______, _______, _______, _______, _____________________LOWER__R3______________________,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

    [_RAISE] = LAYOUT_wrapper(
      _____________________RAISE__L1______________________,                                     _____________________RAISE__R1______________________,
      _____________________RAISE__L2______________________,                                     _____________________RAISE__R2______________________,
      _____________________RAISE__L3______________________, _______, _______, _______, _______, _____________________RAISE__R3______________________,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

    [_SYSTEM] = LAYOUT_wrapper(
      _____________________SYSTEM_L1______________________,                                     _____________________SYSTEM_R1______________________,
      _____________________SYSTEM_L2______________________,                                     _____________________SYSTEM_R2______________________,
      _____________________SYSTEM_L3______________________, _______, _______, _______, _______, _____________________SYSTEM_R3______________________,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

    [_ADJUST] = LAYOUT_wrapper(
      _____________________ADJUST_L1______________________,                                     _____________________ADJUST_R1______________________,
      _____________________ADJUST_L2______________________,                                     _____________________ADJUST_R2______________________,
      _____________________ADJUST_L3______________________, _______, _______, _______, _______, _____________________ADJUST_R3______________________,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),
};

#ifdef OLED_ENABLE

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

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_180;
}

#endif

