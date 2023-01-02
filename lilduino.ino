#define RGB_BUILTIN 2

#include <SPI.h>
#include <SD.h>
#include <stdlib.h>

extern "C" {
    #include "lil.h"
}

void lil_run(lil_t lil, char* source) {
    lil_free_value(lil_parse(lil, source, 0, 1));
}

void output_cb(lil_t lil, const char* string) {
    Serial.printf("%s", string);
}

void error_cb(lil_t lil, size_t pos, const char* msg) {
    Serial.printf("\nLIL error (%u): %s\n", pos, msg);
}

char* readfile_cb(lil_t lil, const char* filename) {
    File f = SD.open(filename, FILE_READ);
    if (!f) {
        char* buf;
        asprintf(&buf, "File %s not found", filename);
        lil_set_error(lil, buf);
        return NULL;
    }
    return strdup(f.readStringUntil('\0').c_str());
}

char* source_cb(lil_t lil, const char* filename) {
    return readfile_cb(lil, filename);
}

void writefile_cb(lil_t lil, const char* filename, const char* contents) {
    File f = SD.open(filename, FILE_WRITE);
    if (!f) {
        char* buf;
        asprintf(&buf, "Error opening %s for writing", filename);
        lil_set_error(lil, buf);
        return;
    }
    f.seek(0);
    size_t towrite = strlen(contents);
    size_t written = f.write((const uint8_t*)contents, towrite);
    f.close();
    if (towrite != written) {
        char* buf;
        asprintf(&buf, "Failed to write to %s (%u/%u bytes successfully written)", filename, written, towrite);
        lil_set_error(lil, buf);
    }
}

lil_t lil;

void setup() {
    // Clear any previous status LED
    neopixelWrite(RGB_BUILTIN, 0, 0, 0);
    
    // Set up ports and filesystem
    Serial.begin(115200);
    if (!SD.begin()) {
        Serial.println("SD initialization failed");
        while (1) {
            // Blink error
            yield();
            neopixelWrite(RGB_BUILTIN, 64, 0, 0);
            delay(500);
            neopixelWrite(RGB_BUILTIN, 0, 0, 0);
            delay(500);
        }
    }
    
    // Set up LIL
    lil = lil_new();
    lil_callback(lil, LIL_CALLBACK_WRITE, (lil_callback_proc_t)output_cb);
    lil_callback(lil, LIL_CALLBACK_ERROR, (lil_callback_proc_t)error_cb);
    lil_callback(lil, LIL_CALLBACK_READ, (lil_callback_proc_t)readfile_cb);
    lil_callback(lil, LIL_CALLBACK_SOURCE, (lil_callback_proc_t)source_cb);
    lil_callback(lil, LIL_CALLBACK_STORE, (lil_callback_proc_t)writefile_cb);
    Serial.println("LIL initialized... running code");

    // Run something
    lil_run(lil, "set file /test.txt; print Writing nonsense to file $file...; store $file [rand]; print LIL script done, file contains [read $file]");
    Serial.println("lil_run() returned, cleaning up");
    lil_free(lil);
}

void loop() {}
