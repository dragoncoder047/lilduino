extern "C" {
    #include "lil.h"
}

void output_cb(lil_t lil, const char* string) {
    Serial.printf("%s", string);
}

void error_cb(lil_t lil, size_t pos, const char* msg) {
    Serial.printf("\nLIL error (%u): %s\n", pos, msg);
}

lil_t lil;

void setup() {
    Serial.begin(115200);
    lil = lil_new();
    lil_callback(lil, LIL_CALLBACK_WRITE, (lil_callback_proc_t)output_cb);
    lil_callback(lil, LIL_CALLBACK_ERROR, (lil_callback_proc_t)error_cb);
    Serial.println("LIL initialized properly... running code");
    lil_free_value(lil_parse(lil, "while 1 {print Hello world!!}", 0, 1));
    // haha, this will never be reached with the above script
    Serial.println("Done, cleaning up");
    lil_free(lil);
    lil = NULL;
}

void loop() {}
