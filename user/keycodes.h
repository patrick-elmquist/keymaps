#pragma once

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)
#define SYSTEM MO(_SYSTEM)

#define QUIT RGUI(KC_Q)
#define CLOSE RGUI(KC_W)
#define RELOAD RGUI(KC_R)

#define ALFRED RGUI(KC_SPC)
#define ITERM RGUI(KC_ESC)

#define LOW_ENT LT(_LOWER, KC_ENT)
#define LOW_SPC LT(_LOWER, KC_SPC)
#define RAI_ESC LT(_RAISE, KC_ESC)
#define RAI_REP LT(_RAISE, REPEAT)
#define RAI_ADP LT(_RAISE, ADAPT)
#define CTL_BSP LCTL_T(KC_BSPC)

// Left-hand home row mods
#define CTL_A LCTL_T(KC_A)
#define ALT_S LALT_T(KC_S)
#define GUI_D LGUI_T(KC_D)
#define SFT_F LSFT_T(KC_F)

// Right-hand home row mods
#define SFT_J RSFT_T(KC_J)
#define GUI_K RGUI_T(KC_K)
#define ALT_L LALT_T(KC_L)
#define CTL_SCLN RCTL_T(KC_SCLN)

#define SFT_G LSFT(KC_G)

enum custom_keycodes {
    OS_LCTL = SAFE_RANGE,
    OS_LALT,
    OS_LGUI,
    OS_LSFT,

    OS_RAIS,
    SW_WIN,
    REPEAT,
    ADAPT,
    QUOTES,
    PARENS,
    BRCKETS,
    CURLYS,

    CAPS,
    CAMEL,
    SNAKE,
    SNK_SCM,

    SE_AO,
    SE_AE,
    SE_OE
};
