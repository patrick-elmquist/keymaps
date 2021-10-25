/*
This is the c configuration file for the keymap

Copyright 2012 Jun Wako <wakojun@gmail.com>
Copyright 2015 Jack Humbert

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#define MASTER_LEFT

#define SPLIT_MODS_ENABLE

#define ONESHOT_TAP_TOGGLE 2
#define ONESHOT_TIMEOUT 3000

#define TAPPING_TERM_PER_KEY
#define TAPPING_FORCE_HOLD_PER_KEY

#define IGNORE_MOD_TAP_INTERRUPT_PER_KEY
#define PERMISSIVE_HOLD_PER_KEY

#define SPLIT_MODS_ENABLE

#ifdef OLED_FONT_H
    #undef OLED_FONT_H
#endif
#define OLED_FONT_H "keyboards/lily58/keymaps/pket/glcdfont.c"
