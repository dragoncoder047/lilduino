#ifndef LILDUINO_FS_FILE
#define LILDUINO_FS_FILE

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
    Serial.printf("\nError at pos %u: %s\n", pos, msg);
}

char* readfile_cb(lil_t lil, const char* filename) {
    File f = SD.open(filename, FILE_READ);
    if (!f) {
        LIL_FAILED(lil, "File %s not found", filename);
        return NULL;
    }
    String s = f.readStringUntil('\0');
    unsigned int sz = s.length();
    char* buf = (char*)malloc(sz + 1);
    s.toCharArray(buf, sz);
    return buf;
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
    if (towrite != written) {
        LIL_FAILED(lil, "Failed to write to %s (%u/%u bytes successfully written)", filename, written, towrite);
    }
}

void lilduino_fs_init(lil_t lil) {
    lil_callback(lil, LIL_CALLBACK_WRITE, (lil_callback_proc_t)output_cb);
    lil_callback(lil, LIL_CALLBACK_ERROR, (lil_callback_proc_t)error_cb);
    lil_callback(lil, LIL_CALLBACK_READ, (lil_callback_proc_t)readfile_cb);
    lil_callback(lil, LIL_CALLBACK_STORE, (lil_callback_proc_t)writefile_cb);
}

#endif
