#ifndef DATA_WIFI
#include <Arduino.h>
#include "WiFi.h"

#define DATA_WIFI 1
struct wifi_t
{
    uint8_t id;
    uint8_t bssid;
    char ssid[50];
    char key[50];
    uint8_t rssi;
    int32_t channel;
    wifi_auth_mode_t encryption_type;
};

#endif