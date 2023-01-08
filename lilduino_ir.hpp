#ifndef LILDUINO_IR_FILE
#define LILDUINO_IR_FILE

#include <Arduino.h>
#include <stdlib.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRremoteESP8266.h>
#include <IRtext.h>
#include <IRutils.h>

extern "C" {
    #include "lil.h"
    #include "lil_helpers.h"
}

lil_value_t fnc_ir_send(lil_t lil, int argc, lil_value_t* argv) {
    LIL_FIXARITY(lil, "ir send", argc, 2);
    const char* protocol_str = lil_to_string(argv[0]);
    uint64_t value = (uint64_t)lil_to_integer(argv[1]);
    decode_type_t protocol = strToDecodeType(protocol_str);
    if (protocol == decode_type_t::UNKNOWN) {
        LIL_FAILED(lil, "unknown IR protocol %s", protocol_str);
        return NULL;
    }
    int sendPin = lil_to_integer(lil_get_var(lil, "ir.txpin"));
    if (sendPin == 0) {
        LIL_FAILED(lil, "ir.txpin is not defined or invalid");
        return NULL;
    }
    IRsend sender(sendPin);
    uint16_t rep = sender.minRepeats(protocol);
    uint16_t bits = sender.defaultBits(protocol);
    if (value >= (1 << bits)) {
        LIL_FAILED(lil, "value of %#llx is too large for protocol %s (max is %#llx)", value, protocol_str, (1 << bits) - 1);
        return NULL;
    }
    sender.begin();
    sender.send(protocol, value, bits, rep);
    return NULL;
}

lil_value_t fnc_ir_receive(lil_t lil, int argc, lil_value_t* argv) {
    LIL_NOARGS(lil, "ir recieve", argc);
    // TODO
}

lil_value_t fnc_ir(lil_t lil, int argc, lil_value_t* argv) {
    LIL_HASSUBCOMMANDS(lil, "ir", argc);
    LIL_SUBCOMMAND(lil, "send", argc, argv, fnc_ir_send);
    LIL_SUBCOMMAND(lil, "receive", argc, argv, fnc_ir_receive);
    LIL_ENDSUBCOMMANDS(lil, "ir", argv);
    return NULL;
}

void lilduino_ir_init(lil_t lil) {
    lil_register(lil, "ir", (lil_func_proc_t)fnc_ir);
}

#endif
