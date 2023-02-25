/*

Some basic arduino functions that don't fit in their own library.

    delay [n] [unit]
      Wait for a period of time
      Units available: s/sec/seconds,
                       m/ms/milliseconds,
                       u/us/microseconds
      Default is 10 milliseconds

More to come!!

*/

#ifndef LILDUINO_BASIC_FILE
#define LILDUINO_BASIC_FILE

#include <Arduino.h>
#include <stdlib.h>

#include "lilduino.h"

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

void lilduino_basic_init(lil_t lil) {
    lil_register(lil, "delay", (lil_func_proc_t)fnc_delay);
}

#endif
