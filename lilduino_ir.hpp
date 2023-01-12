#ifndef LILDUINO_IR_FILE
#define LILDUINO_IR_FILE

#include <Arduino.h>
#include <stdlib.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRremoteESP8266.h>
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
    int txpin = (int)lil_to_integer(lil_get_var(lil, "ir.txpin"));
    if (txpin == 0) {
        LIL_FAILED(lil, "ir.txpin is not defined or invalid (it is %s)", lil_to_string(lil_get_var(lil, "ir.txpin")));
        return NULL;
    }
    IRsend sender(txpin);
    uint16_t rep = sender.minRepeats(protocol);
    uint16_t bits = sender.defaultBits(protocol);
    if (value >= (1 << bits)) {
        LIL_FAILED(lil, "value of %#llx is too large for IR protocol %s (max is %#llx)", value, protocol_str, (1 << bits) - 1);
        return NULL;
    }
    sender.begin();
    sender.send(protocol, value, bits, rep);
    return NULL;
}

IRrecv* RECIEVERS[128] = {NULL}; // 128 pins is more than enough

lil_value_t fnc_ir_receive(lil_t lil, int argc, lil_value_t* argv) {
    LIL_NOARGS(lil, "ir receive", argc);
    int rxpin = (int)lil_to_integer(lil_get_var(lil, "ir.rxpin"));
    if (rxpin == 0) {
        LIL_FAILED(lil, "ir.rxpin is not defined or invalid (it is %s)", lil_to_string(lil_get_var(lil, "ir.rxpin")));
        return NULL;
    }
    IRrecv* reciever = RECIEVERS[rxpin];
    if (reciever == NULL) {
        reciever = new IRrecv(rxpin, 1024, 15, true);
        RECIEVERS[rxpin] = reciever;
        reciever->setTolerance(kTolerance);
        reciever->enableIRIn();
    }
    decode_results results;
    int recieved = reciever->decode(&results);
    if (!recieved) return NULL;
    char* buf;
    asprintf(&buf, "%s %#llx", typeToString(results.decode_type).c_str(), results.value);
    lil_value_t val = lil_alloc_string(buf);
    free(buf);
    return val;
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
