#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include "SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h"
#include <stdlib.h>

#include "lilduino.h"
#include "lilduino_io.hpp"
#include "lilduino_gpio.hpp"
#include "lilduino_regexp.hpp"
#include "lilduino_ir.hpp"
#include "lilduino_battery.hpp"
#include "lilduino_basic.hpp"

#include "misc.hpp"

void lil_run(lil_t lil, char* source) {
    lil_free_value(lil_parse(lil, source, 0, 1));
}

lil_t lil;

SFE_MAX1704X battery(SFE_MAX17048);

int sd_card_failed_blink[3] = {0x000040, 0x400000, 0x000000};

#ifndef yield
#define yield()
#endif

void fatal_status_loop(int d, size_t num, int colors[]) {
    while (1) {
        for (size_t i = 0; i < num; i++) {
            yield();
            status_led(colors[i]);
            delay(d);
        }
    }
}

const char* kernel = R"===(
func repl {} {
    while 1 {
        set aaaaaaaa [input "> "]
        print $aaaaaaaa
        if [streq $aaaaaaaa done] return
        try {
            set bbbbbbbb [upeval $aaaaaaaa]
            if not [streq $bbbbbbbb ""] {print $bbbbbbbb}
        } {
            print Error: [reflect error]
        }
    }
}
try {
    source /main.lil
} {
    print Error running /main.lil: [reflect error]
}
repl
)===";


void setup() {
    // Clear any previous status LED
    status_led(0);
    
    // Set up ports and filesystem
    Serial.begin(115200);
    if (!SD.begin()) {
        Serial.println("SD initialization failed");
        fatal_status_loop(500, 3, sd_card_failed_blink);
    }
    Wire.begin();
    if (!battery.begin()) {
        Serial.println("MAX17048 is down");
        status_led(0x004000);
        delay(500);
        status_led(0x400000);
        delay(500);
        status_led(0);
    }
    
    // Set up LIL
    lil = lil_new();
    lilduino_io_init(lil);
    lilduino_basic_init(lil);
    lilduino_misc_init(lil);
    lilduino_gpio_init(lil);
    lilduino_regexp_init(lil);
    lilduino_ir_init(lil);
    lilduino_battery_init(lil, &battery);

    // Run main file
    Serial.println("LIL initialized...");
    lil_run(lil, (char*)kernel);

    // Clean up
    Serial.println("lil_run() returned, cleaning up");
    lil_free(lil);
}

void loop() {}
