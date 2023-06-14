#include <Arduino.h>
#ifndef _included_data_timezone
#define _included_data_timezone
struct time_zone_t
{
    uint8_t id;
    char name[100];
    char code[100];
};
time_zone_t time_zone_default = {0, "Asia/Jakarta", "WIB-7"};

enum time_state_enum
{
    TIME_STATE_SYNCING,
    TIME_STATE_SYNC_FAILED,
    TIME_STATE_SYNC_SUCCESS,
    TIME_STATE_IDLE
};
#endif