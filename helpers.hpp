#ifndef MY_HELPERS_FILE
#define MY_HELPERS_FILE

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

lil_value_t fnc_wait(lil_t lil, int argc, lil_value_t* argv) {
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
                if (streq(u, "m") || streq(u, "ms") || streq(u, "milliseconds")) {
                    unit = 'm';
                }
                else if (streq(u, "u") || streq(u, "us") || streq(u, "microseconds")) {
                    unit = 'u';
                }
                else if (streq(u, "s") || streq(u, "sec") || streq(u, "seconds")) {
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
            LIL_FAILED(lil, "Too many args to wait (expected 0-2, got %i)", argc);
    }
    return NULL;
}

lil_value_t fnc_status(lil_t lil, int argc, lil_value_t* argv) {
    if (argc != 3) {
        LIL_FAILED(lil, "Expected 3 args to status, got %i", argc);
        return NULL;
    }
    int r = (int)lil_to_integer(argv[0]) & 0xFF;
    int g = (int)lil_to_integer(argv[1]) & 0xFF;
    int b = (int)lil_to_integer(argv[2]) & 0xFF;
    int c = (r << 16) + (g << 8) + b;
    status_led(c);
    return NULL;
}

lil_value_t fnc_input(lil_t lil, int argc, lil_value_t* argv) {
    for (int i = 0; i < argc; i++) {
        lil_write(lil, lil_to_string(argv[i]));
        if (i + 1 != argc) lil_write(lil, " ");
    }
    return lil_alloc_string(Serial.readStringUntil('\n').c_str());
}

void lilduino_helpers_init(lil_t lil) {
    lil_register(lil, "wait", (lil_func_proc_t)fnc_wait);
    lil_register(lil, "status", (lil_func_proc_t)fnc_status);
    lil_register(lil, "input", (lil_func_proc_t)fnc_input);
}

#endif
