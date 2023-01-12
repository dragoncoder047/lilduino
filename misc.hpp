/*

Some random functions I thought would be useful. They rely heavily on
the hardware on my board (an ESP32); don't expect them all to work on your board.

    delay [n] [unit]
      Wait for a period of time
      Units available: s/sec/seconds,
                       m/ms/milliseconds,
                       u/us/microseconds
      Default is 10 milliseconds

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

#ifndef yield
#define yield()
#endif

void status_led(int color) {
    int r = color >> 16 & 0xFF;
    int g = color >> 8  & 0xFF;
    int b = color       & 0xFF;
    neopixelWrite(RGB_BUILTIN, r, g, b);
}

void fatal_status_loop(int d, size_t num, int colors[]) {
    while (1) {
        for (size_t i = 0; i < num; i++) {
            yield();
            status_led(colors[i]);
            delay(d);
        }
    }
}

lil_value_t fnc_delay(lil_t lil, int argc, lil_value_t* argv) {
    LIL_CHECKARGS(lil, "delay", argc, 0, 2);
    switch (argc) {
        case 0:
            delay(10); // Debouncing call
            break;
        case 1:
        case 2: {
            int num = (int)lil_to_integer(argv[0]);
            char unit = 'm';
            if (argc == 2) {
                const char* u = lil_to_string(argv[1]);
                if (streq(u, "m") || streq(u, "ms") || streq(u, "milliseconds") || streq(u, "millisecond")) {
                    unit = 'm';
                }
                else if (streq(u, "u") || streq(u, "us") || streq(u, "microseconds") || streq(u, "microsecond")) {
                    unit = 'u';
                }
                else if (streq(u, "s") || streq(u, "sec") || streq(u, "seconds") || streq(u, "second")) {
                    unit = 's';
                }
                else {
                    LIL_FAILED(lil, "Unknown unit for wait: %s", u);
                    return NULL;
                }
            }
            switch (unit) {
                case 'm': delay(num); break;
                case 'u': delayMicroseconds(num);
                case 's': delay(1000 * num); break;
                default: break; // unreachable
            }
            break;
        }
        default:
            break; // unreachable
    }
    return NULL;
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

void lilduino_helpers_init(lil_t lil) {
    lil_register(lil, "delay", (lil_func_proc_t)fnc_delay);
    lil_register(lil, "status", (lil_func_proc_t)fnc_status);
}

#endif
