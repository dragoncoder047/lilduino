#ifndef LILDUINO_GPIO_FILE
#define LILDUINO_GPIO_FILE

#include <Arduino.h>
#include <stdlib.h>

extern "C" {
    #include "lil.h"
    #include "lil_helpers.h"
}

lil_value_t fnc_gpio_pinmode(lil_t lil, int argc, lil_value_t* argv) {
    LIL_FIXARITY(lil, "gpio::pinmode", 2);
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
    LIL_FIXARITY(lil, "gpio::digitalread", 1);
    int pin_no = (int)lil_to_integer(argv[0]);
    return lil_alloc_integer(digitalRead(pin_no));
}

lil_value_t fnc_gpio_digitalwrite(lil_t lil, int argc, lil_value_t* argv) {
    LIL_FIXARITY(lil, "gpio::digitalwrite", 2);
    int pin_no = (int)lil_to_integer(argv[0]);
    const char* what = lil_to_string(argv[1]);
    digitalWrite(pin_no, streq(what, "HIGH") || streq(what, "1") ? HIGH : LOW);
    return NULL;
}

lil_value_t fnc_gpio_analogread(lil_t lil, int argc, lil_value_t* argv) {
    LIL_FIXARITY(lil, "gpio::analogread", 1);
    int pin_no = (int)lil_to_integer(argv[0]);
    return lil_alloc_integer(analogRead(pin_no));
}

lil_value_t fnc_gpio_analogwrite(lil_t lil, int argc, lil_value_t* argv) {
    LIL_FIXARITY(lil, "gpio::analogwrite", 2);
    int pin_no = (int)lil_to_integer(argv[0]);
    int value = (int)lil_to_integer(argv[0]);
    analogWrite(pin_no, value);
    return NULL;
}

void lilduino_gpio_init(lil_t lil) {
    lil_register(lil, "gpio::pinmode", (lil_func_proc_t)fnc_gpio_pinmode);
    lil_register(lil, "gpio::digitalread", (lil_func_proc_t)fnc_gpio_digitalread);
    lil_register(lil, "gpio::digitalwrite", (lil_func_proc_t)fnc_gpio_digitalwrite);
    lil_register(lil, "gpio::analogread", (lil_func_proc_t)fnc_gpio_analogread);
    lil_register(lil, "gpio::analogwrite", (lil_func_proc_t)fnc_gpio_analogwrite);
}

#endif
