#include <Arduino.h>
#include "time_zone_t.h"

#ifndef _included_internet_state
#define _included_internet_state 1
enum internet_state_enum
{
    INTERNET_STATE_CONNECTED,
    INTERNET_STATE_NOT_CONNECTED,
    INTERNET_STATE_WAITING
};
enum wifi_state_enum
{
    WIFI_STATE_NOT_CONNECTED,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_CONNECT_FAILED,
    WIFI_STATE_SCANNING,
    WIFI_STATE_SCAN_SUCCESS,
    WIFI_STATE_SCAN_FAILED
};
enum fetch_state_enum
{
    FETCH_IDLE,
    FETCH_WAIT,
    FETCH_SETTLED,
    FETCH_TIMEOUT
};
#endif