/*

Implements something like the Tcl "clock" command.

    clock [milliseconds|ms]
    clock [microseconds|us]
    clock [seconds|secs]
      Returns millis(), micros(), or 1000*millis(), respectively.
      Note that the "epoch" (the zero-time moment) is whenever your
      arduino last restarted, not Jan 1, 1970 00:00 UTC. These values
      are only sutable for timing the duration of something.
    
    clock format <format> <time>
      Format the time stamp into a human readable representation.
      See strftime(3) man page for details.
      Note the "epoch" here IS Jan 1, 1970 00:00 UTC!!

*/

#ifndef LILDUINO_CLOCK_FILE
#define LILDUINO_CLOCK_FILE

#include <Arduino.h>
#include "time.h"

extern "C" {
    #include "lil.h"
    #include "lil_helpers.h"
}

lil_value_t fnc_clock_milliseconds(lil_t lil, int argc, lil_value_t* args) {
    LIL_NOARGS(lil, "clock milliseconds", argc);
    return lil_alloc_integer((lilint_t)millis());
}

lil_value_t fnc_clock_microseconds(lil_t lil, int argc, lil_value_t* args) {
    LIL_NOARGS(lil, "clock microseconds", argc);
    return lil_alloc_integer((lilint_t)micros());
}

lil_value_t fnc_clock_seconds(lil_t lil, int argc, lil_value_t* args) {
    LIL_NOARGS(lil, "clock seconds", argc);
    return lil_alloc_integer((lilint_t)(1000 * millis()));
}

lil_value_t fnc_clock_format(lil_t lil, int argc, lil_value_t* argv) {
    LIL_FIXARITY(lil, "clock format", argc, 2);
    const char* fmt = lil_to_string(argv[0]);
    time_t t = (time_t)lil_to_integer(argv[1]);
    struct tm* tbits = localtime(&t);
    char buf[256];
    strftime(buf, 255, fmt, tbits);
    return lil_alloc_string(buf);
}

lil_value_t fnc_clock(lil_t lil, int argc, lil_value_t* argv) {
    LIL_HASSUBCOMMANDS(lil, "clock", argc);
    LIL_SUBCOMMAND(lil, "milliseconds", argc, argv, fnc_clock_milliseconds);
    LIL_SUBCOMMAND(lil, "ms", argc, argv, fnc_clock_milliseconds);
    LIL_SUBCOMMAND(lil, "microseconds", argc, argv, fnc_clock_microseconds);
    LIL_SUBCOMMAND(lil, "us", argc, argv, fnc_clock_microseconds);
    LIL_SUBCOMMAND(lil, "seconds", argc, argv, fnc_clock_seconds);
    LIL_SUBCOMMAND(lil, "secs", argc, argv, fnc_clock_seconds);
    LIL_SUBCOMMAND(lil, "format", argc, argv, fnc_clock_format);
    LIL_ENDSUBCOMMANDS(lil, "clock", argv);
}

void lilduino_clock_init(lil_t lil) {
    lil_register(lil, "clock", (lil_func_proc_t)fnc_clock);
}

#endif
