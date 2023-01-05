# LILduino

Quick-and-dirty example of using Kostas Michalopoulos' [LIL](http://runtimeterror.com/tech/lil/) programming language on a microcontroller, using Arduino.

The currently included sources and readme are for LIL 20211019 which was the latest version as of when I wrote this (but see the [section at the bottom](#lil-patches), I made a few patches).

I tested it on an ESP32 and it ran perfectly without errors (that is, any segfaults or such. If I write bad code, it tells me so.) No guarantee for other platforms.

## Extension files (`lilduino_*.hpp`)

### `helpers.hpp` -- TESTED

**Loader function:** `lilduino_helpers_init(lil)`

```tcl
wait [n] [unit]
  # Wait for a period of time
  # Units available: s/sec/seconds,
  #                  m/ms/milliseconds,
  #                  u/us/microseconds
  # Default is 10 milliseconds

status <color>
status <r> <g> <b>
  # Set the color of the on-board WS2182 status LED
  # Color is a 24-bit 0xRRGGBB form

input [prompt]
  # Print the prompt and return the next
  # line of input from the serial port
```

### `lilduino_fs.hpp` -- TESTED

**Loader function:** `lilduino_fs_init(lil)`

This also sets up `store`, `read`, and `source` to use the Arduino SD card file system, and `write` to use the `Serial` port, as well as directing unhandled errors to the serial port.

```tcl
mkdir <dir>
  # Creates the directory on the SD card
  # Does nothing if it already exists

rm <file>
  # Deletes the file from the SD card

rmdir <directory>
  # Removes a directory
  # Will fail if the directory isn't empty
```

### `lilduino_regexp.hpp` -- UNTESTED

**Loader function:** `lilduino_regexp_init(lil)`

Depends on [Nick Gammon's regular expression library](https://github.com/nickgammon/Regexp), make sure you have that installed.

```tcl
re::match <regexp> <string> [var1 var2 ...]
  # Matches the regular expression against the string.
  # If any vars are provided, they are filled with the capture
  # groups present in the regular expression.
  # Providing more variables than capture groups will result in
  # undefined behavior (at best, the offending variables get
  # filled with garbage, at worst, your microcontroller will segfault).
  # Returns the entire match, empty string if no match.

re::replace <regexp> <string> <replacement> [maxrepl]
  # Replaces all matches of the regexp in the string with the
  # replacement string (no support for back referencing capture groups yet).
  # If maxrepl is given and > 0, it limits the maximum number of replacements to be made.
```

### `lilduino_wifi.hpp` -- UNTESTED

**Loader function:** `lilduino_wifi_init(lil)`

```tcl
wifi::mode <mode>
  # Mode is one of "device", "server", or "proxy".
  # Sets up the mode of operation of the wifi module.
  # "device" -> as a wifi station, same behavior as your phone or laptop
  # "server" -> as a wifi access point, which your phone or laptop can connect to
  # "proxy" -> both an access point and station

wifi::connect <ssid> <password> [timeout]
  # Connect to the named wifi network.
  # Only makes sense if a previous "wifi::mode device"
  # or "wifi::mode proxy" command was issued.
  # Timeout is in seconds and defaults to 10.
  # Returns 1 if connected, 0 if timed out.

wifi::softap <ssid> <password>
  # Sets up the SSID and password of the broadcasted network.
  # Only makes sense if a previous "wifi::mode server"
  # or "wifi::mode proxy" command was issued.

wifi::mac
  # Returns the device's MAC address as a xx:xx:xx:xx:xx:xx string.

wifi::ip [ip]
  # If no IP address is provided, returns the device's
  # current (manually or automatically) configured IP address.
  # If an IP address is given, it sets this device's local IP to the one provided.
```

### `lilduino_gpio.hpp` -- UNTESTED

**Loader function:** `lilduino_gpio_init(lil)`

```tcl
gpio::pinmode <pin> <mode>
  # Configures the I/O mode of the specified pin.
  # Mode can be "input", "output", "input_pullup", or "input_pulldown"
  # (the last is only available on some platforms)

gpio::digitalread <pin>
  # Reads the digital state of the pin and
  # returns 1 if high, 0 if low.

gpio::digitalwrite <pin> <state>
  # Writes the state to the output pin.
  # The string "HIGH" and 1 are both interpreted
  # as high, anything else is low.

gpio::analogread <pin>
  # Reads the analog voltage on the input pin and
  # returns a number corresponding to the voltage.
  # On ESP32 the ADC is 12 bits, 0-4095.

gpio::analogwrite <pin> <duty cycle>
  # Sets up PWM on the output pin with the
  # provided duty cycle (0-255 resolution).

gpio::touchread <pin>
  # On devices with capacitive-touch hardware,
  # runs the touch detector on the pin and returns the counter value.
  # Lower values mean the pin's electrode is being touched harder.
```

## LIL Patches

* Better number parsing: `lil_to_double()` and `lil_to_integer()` patched to allow hexadecimal and octal numbers instead of only decimal (by using `sscanf()` instead of `atof()` / `atoll()`)
* Deleted all memory pools code (`LIL_ENABLE_POOLS`) because it is useless on a microcontroller.

## Notes

* `mine` is a little bash script that makes the current user the owner of all files in the current working directory. I have it because for some reason I have to run the arduino IDE with `sudo` for it to work and as such when I save a file it changes the owner to `root` and then I can't edit the file anywhere else. Thus that script.

* LIL is licensed under the zlib license.
