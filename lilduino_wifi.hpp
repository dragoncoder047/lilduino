#ifndef LILDUINO_WIFI_FILE
#define LILDUINO_WIFI_FILE

#include <Arduino.h>
#include <WiFi.h>
#include <stdlib.h>

extern "C" {
    #include "lil.h"
    #include "lil_helpers.h"
}

lil_value_t fnc_wifi_mode(lil_t lil, int argc, lil_value_t* argv) {
    LIL_FIXARITY(lil, "wifi::mode", argc, 1);
    const char* mode = lil_to_string(argv[0]);
    if (streq(mode, "device")) WiFi.mode(WIFI_STA);
    else if (streq(mode, "server")) WiFi.mode(WIFI_AP);
    else if (streq(mode, "proxy")) WiFi.mode(WIFI_STA_AP);
    else LIL_FAILED(lil, "invalid mode for wifi::mode (%s not one of device, server, proxy)", mode);
    return NULL;
}

lil_value_t fnc_wifi_connect(lil_t lil, int argc, lil_value_t* argv) {
    LIL_CHECKARGS(lil, "wifi::connect", argc, 2, 3);
    const char* ssid = lil_to_string(argv[0]);
    const char* pass = lil_to_string(argv[1]);
    int timeout = 10000;
    if (argc == 3) timeout = 1000 * lil_to_integer(argv[3]);
    WiFi.begin(ssid, pass);
    unsigned long now = millis();
    return lil_alloc_integer(WiFi.waitForConnectResult() == WL_CONNECTED ? 1 : 0);
}

lil_value_t fnc_wifi_config(lil_t lil, int argc, lil_value_t* argv) {
    LIL_FIXARITY(lil, "wifi::setupnet", argc, 2);
    const char* ssid = lil_to_string(argv[0]);
    const char* pass = lil_to_string(argv[1]);
    bool ok = WiFi.softAP(ssid, pass);
    if (!ok) LIL_FAILED(lil, "Failed to configure wifi access point network");
    return NULL;
}

lil_value_t fnc_wifi_mac(lil_t lil, int argc, lil_value_t* args) {
    LIL_NOARGS(lil, "wifi::mac", argc);
    uint8_t mac[6];
    char macstr[18];
    WiFi.macAddress(mac);
    snprintf(macstr, 18, "%x:%x:%x:%x:%x:%x", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
    return lil_alloc_string(macstr);
}

lil_value_t fnc_wifi_ip(lil_t lil, int argc, lil_value_t* args) {
    LIL_CHECKARGS(lil, "wifi::ip", argc, 0, 1);
    if (argc == 1) { // Set my IP
        const char* ipstr = lil_to_string(argv[0]);
        uint8_t ip[4] = {192, 168, 1, 158};
        sscanf(ipstr, "%i:%i:%i:%i", &ip[0], &ip[1], &ip[2], &ip[3]);
        WiFi.config(ip);
        return NULL;
    } else { // Get local IP
        return lil_alloc_string(WiFi.localIP().toString().c_str());
    }
}

void lilduino_wifi_init(lil_t lil) {
    lil_register(lil, "wifi::mode", (lil_func_proc_t)fnc_wifi_mode);
    lil_register(lil, "wifi::connect", (lil_func_proc_t)fnc_wifi_connect);
    lil_register(lil, "wifi::mac", (lil_func_proc_t)fnc_wifi_mac);
    lil_register(lil, "wifi::ip", (lil_func_proc_t)fnc_wifi_ip);
}

#endif
