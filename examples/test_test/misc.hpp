/*
    status <color>
    status <r> <g> <b>
      Set the color of the on-board WS2182 status LED
      Color is a 24-bit 0xRRGGBB form

*/

#ifndef MISC_FILE
#define MISC_FILE

#include <Arduino.h>
#include <stdlib.h>

extern "C" {
    #include "lil.h"
    #include "lil_helpers.h"
}

#ifndef RGB_BUILTIN
#define RGB_BUILTIN 2
#endif

void status_led(int color) {
    int r = color >> 16 & 0xFF;
    int g = color >> 8  & 0xFF;
    int b = color       & 0xFF;
    neopixelWrite(RGB_BUILTIN, r, g, b);
}

lil_value_t fnc_status(lil_t lil, int argc, lil_value_t* argv) {
    LIL_CHECKARGS(lil, "status", argc, 1, 3);
    if (argc == 2) {
        LIL_FAILED(lil, "Can only have 1 or 3 args to status (got 2)");
        return NULL;
    }
    int r = (int)lil_to_integer(argv[0]);
    if (argc == 3) {
        r &= 0xFF;
        int g = (int)lil_to_integer(argv[1]) & 0xFF;
        int b = (int)lil_to_integer(argv[2]) & 0xFF;
        r = (r << 16) + (g << 8) + b;
    }
    status_led(r);
    return NULL;
}

void lilduino_misc_init(lil_t lil) {
    lil_register(lil, "status", (lil_func_proc_t)fnc_status);
}

#endif
