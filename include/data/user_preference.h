#include <Arduino.h>

#ifndef USER_PREFERENCE_T
#define USER_PREFERENCE_T 1
struct user_preference_t
{
    char wifi_ssid[100];
    char wifi_key[100];
    uint8_t time_zone_id;
    char time_zone_name[100];
    char time_zone_code[100];
};
#define USER_PREF_KEY "usr_pref"
#define WIFI_SSID_BOOT_KEY "w_sid_bt"
#define WIFI_KEY_BOOT_KEY "w_ky_bt"
#define TIME_ZONE_CODE_BOOT_KEY "tz_cd_bt"
#define TIME_ZONE_NAME_BOOT_KEY "tz_nm_bt"
#define TIME_ZONE_ID_BOOT_KEY "tz_id_bt"
#define USER_PREFERENCE_ADDRESS 0
#endif