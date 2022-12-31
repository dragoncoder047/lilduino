# lilduino

Quick-and-dirty example of using Kostas Michalopoulos' [LIL] programming language
on a microcontroller, using Arduino.

The currently included sources and readme are for LIL 20211019 which was the latest version as of when I wrote this.

I tested it on an ESP32 and it ran perfectly without errors (that is, any segfaults or such. If I write bad code, it tells me so.)

All that's implemented right now is standard output (by way of `Serial`) and error reporting.
More functionality is forthcoming.

[LIL]: http://runtimeterror.com/tech/lil/

---

`mine` is a little bash script that makes the current user the owner of all files in the current working directory. I have it because for some reason I have to run the arduino IDE with `sudo` for it to work and as such when I save a file it changes the owner to `root` and then I can't edit the file anywhere else. Thus that script.

---

*LIL is licensed under the zlib license.*
