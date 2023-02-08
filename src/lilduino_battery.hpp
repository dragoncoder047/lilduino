/*

Interface to a MAX1704x lithium battery charge gauge IC.

Dependency: https://github.com/sparkfun/SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library/

Note that the loader function for this expects you to pass in a pointer to the battery
as well as the LIL interpreter.

    battery voltage
      Returns the battery voltage, as a floating point number.
    
    battery percentage
      Returns the estimated percentage of energy left in the battery.
    
    battery rate
      Returns the rate at which the battery is changing (% per hour).
    
    battery is_low
      Returns true if the IC has flagged that the battery is low.

*/

#ifndef LILDUINO_BATTERY_FILE
#define LILDUINO_BATTERY_FILE

#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>

#include "lilduino.h"

SFE_MAX1704X* the_battery;

#define _batt_func(name, bcall, ta, tc) \
    lil_value_t fnc_battery_##name(lil_t lil, int argc, lil_value_t* argv) { \
        LIL_NOARGS(lil, "battery " #name, argc); \
        tc val = (tc)the_battery->bcall(); \
        return lil_alloc_##ta(val); \
    }

_batt_func(voltage, getVoltage, double, double)
_batt_func(percentage, getSOC, double, double)
_batt_func(rate, getChangeRate, double, double)
_batt_func(is_low, isLow, integer, lilint_t);

lil_value_t fnc_battery(lil_t lil, int argc, lil_value_t* argv) {
    LIL_HASSUBCOMMANDS(lil, "battery", argc);
    LIL_SUBCOMMAND(lil, "voltage", argc, argv, fnc_battery_voltage);
    LIL_SUBCOMMAND(lil, "percentage", argc, argv, fnc_battery_percentage);
    LIL_SUBCOMMAND(lil, "rate", argc, argv, fnc_battery_rate);
    LIL_SUBCOMMAND(lil, "is_low", argc, argv, fnc_battery_is_low);
    LIL_ENDSUBCOMMANDS(lil, "battery", argv);
    return NULL;
}

void lilduino_battery_init(lil_t lil, SFE_MAX1704X* b) {
    the_battery = b;
    lil_register(lil, "battery", (lil_func_proc_t)fnc_battery);
}

#endif
