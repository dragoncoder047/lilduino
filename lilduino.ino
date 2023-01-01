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
    unsigned long sz = f.available();
    char* buf = (char*) malloc(sz + 1);
    f.read((uint8_t*)buf, sz);
    return buf;
}

char* source_cb(lil_t lil, const char* filename) {
    return readfile_cb(lil, filename);
}

void writefile_cb(lil_t lil, const char* filename, const char* contents) {
    File f = SD.open(filename, FILE_WRITE);
    if (!f) goto ERROR;
    f.seek(0);
    f.write((const uint8_t*)contents, strlen(contents));
    return;
    ERROR:
    char* buf;
    asprintf(&buf, "Error writing to file %s", filename);
    lil_set_error(lil, buf);
}

lil_t lil;

void setup() {
    // Set up ports and filesystem
    Serial.begin(115200);
    if (!SD.begin()) {
        Serial.println("SD initialization failed");
        while (1) {
            // Blink error
            yield();
            neopixelWrite(2, 64, 0, 0);
            delay(500);
            neopixelWrite(2, 0, 0, 0);
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
    lil_run(lil, "print Writing nonsense to file test.txt...; store test.txt [rand]; print LIL script done");
    Serial.println("lil_run() returned, cleaning up");
    lil_free(lil);
}

void loop() {}
