#include <Arduino.h>

#ifndef DATA_TERAPI
#define DATA_TERAPI 1
enum TerapiState
{
    TERAPI_INIT,
    TERAPI_READY,
    TERAPI_RUNNING,
    TERAPI_FINISHED,
    TERAPI_STOP
};
struct data_terapi_t
{
    uint8_t terapi_param_state = TERAPI_INIT;
    float terapi_param_calory = 0.0;
    float terapi_param_o2 = 0.0;
    uint32_t terapi_param_rotation = 0;
    uint32_t terapi_param_heartrate = 0;
    uint32_t terapi_param_heart_count = 0;
    uint32_t terapi_param_time = 0;
    unsigned long time_start;
    unsigned long time_end;
};

#endif