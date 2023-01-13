/*

This module sets up store, read, and source to use the
Arduino SD card file system, and write to use the serial port,
as well as directing unhandled errors to the serial port, and sets
up receiving a ~ when nothing is expected causes a "keyboard interrupt".

    mkdir <dir>
      Creates the directory on the SD card.
      Does nothing if the directory already exists.

    rm <file>
      Deletes the file from the SD card.
      Does nothing if the file doesn't exist.

    rmdir <directory>
      Removes a directory.
      Will fail silently if the directory isn't empty.

    input [prompt]
      Print the prompt and return the next
      line of input from the serial port.

*/

#ifndef LILDUINO_IO_FILE
#define LILDUINO_IO_FILE

#include <Arduino.h>
#include <SD.h>

extern "C" {
    #include "lil.h"
    #include "lil_helpers.h"
}

void output_cb(lil_t lil, const char* string) {
    Serial.printf("%s", string);
}

void error_cb(lil_t lil, size_t pos, const char* msg) {
    Serial.printf("\nUnhandled error at pos %u: %s\n", pos, msg);
}

char* readfile_cb(lil_t lil, const char* filename) {
    File f = SD.open(filename, FILE_READ);
    if (!f) {
        LIL_FAILED(lil, "File %s not found", filename);
        return strdup("");
    }
    return strdup(f.readStringUntil(0).c_str());
}

void writefile_cb(lil_t lil, const char* filename, const char* contents) {
    File f = SD.open(filename, FILE_WRITE);
    if (!f) {
        LIL_FAILED(lil, "Error opening %s for writing", filename);
        return;
    }
    f.seek(0);
    size_t towrite = strlen(contents);
    size_t written = f.write((const uint8_t*)contents, towrite);
    f.close();
    if (towrite != written) LIL_FAILED(lil, "Failed to write to %s (%u/%u bytes successfully written)", filename, written, towrite);
}

void interrupt_cb(lil_t lil) {
    if (Serial.available() > 0 && Serial.peek() == '~') {
        Serial.read();
        LIL_FAILED(lil, "Keyboard interrupt");
    }
}

#define _sdfun(name, sdfun) \
    lil_value_t fnc_##name(lil_t lil, int argc, lil_value_t* argv) { \
        LIL_FIXARITY(lil, #name, argc, 1); \
        SD.sdfun(lil_to_string(argv[0])); \
        return NULL; \
    } \

_sdfun(mkdir, mkdir)
_sdfun(rm, remove)
_sdfun(rmdir, rmdir)

#define input_CHUNK 32
lil_value_t fnc_input(lil_t lil, int argc, lil_value_t* argv) {
    for (int i = 0; i < argc; i++) {
        lil_write(lil, lil_to_string(argv[i]));
        if (i + 1 != argc) lil_write(lil, " ");
    }
    size_t sz = input_CHUNK;
    char* buf = (char*) malloc(sz);
    if (buf == NULL) {
        LIL_FAILED(lil, "Out of memory");
        return NULL;
    }
    size_t ptr = 0;
    while (true) {
        yield();
        if (Serial.available() > 0) {
            char c = Serial.read();
            if (c == '\n') break;
            ptr++;
            if (ptr == sz) {
                sz += input_CHUNK;
                buf = (char*) realloc((void*)buf, sz);
                if (buf == NULL) {
                    LIL_FAILED(lil, "Out of memory");
                    return NULL;
                }
            }
            buf[ptr] = c;
        }
    }
    lil_value_t val = lil_alloc_string(buf);
    free(buf);
    return val;
}

void lilduino_io_init(lil_t lil) {
    lil_callback(lil, LIL_CALLBACK_WRITE, (lil_callback_proc_t)output_cb);
    lil_callback(lil, LIL_CALLBACK_ERROR, (lil_callback_proc_t)error_cb);
    lil_callback(lil, LIL_CALLBACK_READ, (lil_callback_proc_t)readfile_cb);
    lil_callback(lil, LIL_CALLBACK_STORE, (lil_callback_proc_t)writefile_cb);
    lil_callback(lil, LIL_CALLBACK_CHECKINTERRUPT, (lil_callback_proc_t)interrupt_cb);
    lil_register(lil, "mkdir", (lil_func_proc_t)fnc_mkdir);
    lil_register(lil, "rm", (lil_func_proc_t)fnc_rm);
    lil_register(lil, "rmdir", (lil_func_proc_t)fnc_rmdir);
    lil_register(lil, "input", (lil_func_proc_t)fnc_input);
}

#endif
