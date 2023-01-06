#ifndef LILDUINO_GPIO_FILE
#define LILDUINO_GPIO_FILE

#include <Arduino.h>
#include <stdlib.h>

extern "C" {
    #include "lil.h"
    #include "lil_helpers.h"
}

lil_value_t fnc_gpio_pinmode(lil_t lil, int argc, lil_value_t* argv) {
    LIL_FIXARITY(lil, "gpio pinmode", argc, 2);
    int pin_no = (int)lil_to_integer(argv[0]);
    const char* mode = lil_to_string(argv[1]);
    if (streq(mode, "input")) pinMode(pin_no, INPUT);
    else if (streq(mode, "output")) pinMode(pin_no, OUTPUT);
    else if (streq(mode, "input_pullup")) pinMode(pin_no, INPUT_PULLUP);
    #ifdef INPUT_PULLDOWN
    else if (streq(mode, "input_pulldown")) pinMode(pin_no, INPUT_PULLDOWN);
    #endif
    else LIL_FAILED(lil,
                    "Invalid pin mode %s (acceptable are input, output, input_pullup"
                    #ifdef INPUT_PULLDOWN
                    ", input_pulldown"
                    #endif
                    ")", mode);
    return NULL;
}

lil_value_t fnc_gpio_digitalread(lil_t lil, int argc, lil_value_t* argv) {
    LIL_FIXARITY(lil, "gpio digitalread", argc, 1);
    int pin_no = (int)lil_to_integer(argv[0]);
    return lil_alloc_integer(digitalRead(pin_no));
}

lil_value_t fnc_gpio_digitalwrite(lil_t lil, int argc, lil_value_t* argv) {
    LIL_FIXARITY(lil, "gpio digitalwrite", argc, 2);
    int pin_no = (int)lil_to_integer(argv[0]);
    const char* what = lil_to_string(argv[1]);
    digitalWrite(pin_no, streq(what, "LOW") || streq(what, "low") ? LOW : lil_to_boolean(argv[1]));
    return NULL;
}

lil_value_t fnc_gpio_analogread(lil_t lil, int argc, lil_value_t* argv) {
    LIL_FIXARITY(lil, "gpio analogread", argc, 1);
    int pin_no = (int)lil_to_integer(argv[0]);
    return lil_alloc_integer(analogRead(pin_no));
}

lil_value_t fnc_gpio_analogwrite(lil_t lil, int argc, lil_value_t* argv) {
    LIL_FIXARITY(lil, "gpio analogwrite", argc, 2);
    int pin_no = (int)lil_to_integer(argv[0]);
    int value = (int)lil_to_integer(argv[0]);
    analogWrite(pin_no, value);
    return NULL;
}

#ifdef touchRead
lil_value_t fnc_gpio_touchread(lil_t lil, int argc, lil_value_t* argv) {
    LIL_FIXARITY(lil, "gpio touchread", argc, 1);
    int pin_no = (int)lil_to_integer(argv[0]);
    return lil_alloc_integer(touchRead(pin_no));
}
#endif

lil_value_t fnc_gpio(lil_t lil, int argc, lil_value_t* argv) {
    LIL_HASSUBCOMMANDS(lil, "gpio", argc);
    LIL_SUBCOMMAND(lil, "pinmode", argc, argv, fnc_gpio_pinmode);
    LIL_SUBCOMMAND(lil, "digitalread", argc, argv, fnc_gpio_digitalread);
    LIL_SUBCOMMAND(lil, "digitalwrite", argc, argv, fnc_gpio_digitalwrite);
    LIL_SUBCOMMAND(lil, "analogread", argc, argv, fnc_gpio_analogread);
    LIL_SUBCOMMAND(lil, "analogwrite", argc, argv, fnc_gpio_analogwrite);
    #ifdef touchRead
    LIL_SUBCOMMAND(lil, "touchread", argc, argv, fnc_gpio_touchread);
    #endif
    LIL_ENDSUBCOMMANDS(lil, "gpio", argv);
}

void lilduino_gpio_init(lil_t lil) {
    lil_register(lil, "gpio", (lil_func_proc_t)fnc_gpio);
}

#endif
