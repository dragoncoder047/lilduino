# LILduino

Quick-and-dirty example of using Kostas Michalopoulos' [LIL](http://runtimeterror.com/tech/lil/) programming language on a microcontroller, using Arduino.

The currently included sources and readme are for LIL 20211019 which was the latest version as of when I wrote this (but see the [section at the bottom](#lil-patches), I made a few patches).

I tested it on an ESP32 and it ran perfectly without errors (that is, any segfaults or such. If I write bad code, it tells me so.) No guarantee for other platforms.

Documentation included at the top of each extension file.

## LIL Patches

* Better number parsing - patched to allow hexadecimal and octal numbers instead of only decimal (by using `sscanf()` instead of `atof()` / `atoll()` and custom code in `ee_numeric_element()`)
* Deleted all memory pools code (`LIL_ENABLE_POOLS`) because it is useless on a microcontroller.
* Added fast number types into the `_lil_value_t` struct to take advantage of hardware floating point support and reduce the number of string&rarr;number conversions, increasing speed and reliablilty.

## Notes

* `mine` is a little bash script that makes the current user the owner of all files in the current working directory. I have it because for some reason I have to run the arduino IDE with `sudo` for it to work and as such when I save a file it changes the owner to `root` and then I can't edit the file anywhere else. Thus that script.

* LIL is licensed under the zlib license.
