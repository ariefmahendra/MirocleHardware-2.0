#ifndef _included_preference_manager
#define _included_preference_manager

#include "EEPROM.h"
#include "Preferences.h"
#include "data/user_preference.h"
#include "data/data_time_zone.h"
#include "config.h"

user_preference_t user_preference;
Preferences preference_manager;
String wifi_ssid_boot, wifi_key_boot;
String time_zone_name_boot, time_zone_code_boot;
bool user_preference_init();
void user_preference_update();

bool user_preference_init()
{
    preference_manager.begin(USER_PREF_KEY, false);

    user_preference_update();

    if (String(user_preference.time_zone_code) == "" || String(user_preference.time_zone_name) == "")
    {
#if _PREFERENCE_MANAGER_DEBUG
        Serial.println("init timezone boot");
#endif
        preference_manager.putUInt(TIME_ZONE_ID_BOOT_KEY, time_zone_default.id);
        preference_manager.putString(TIME_ZONE_NAME_BOOT_KEY, String(time_zone_default.name));
        preference_manager.putString(TIME_ZONE_CODE_BOOT_KEY, String(time_zone_default.code));
    }
    return true;
}
void user_preference_update()
{
    preference_manager.getString(WIFI_SSID_BOOT_KEY, "").toCharArray(user_preference.wifi_ssid, 100, 0);
    preference_manager.getString(WIFI_KEY_BOOT_KEY, "").toCharArray(user_preference.wifi_key, 100, 0);

    user_preference.time_zone_id = preference_manager.getUInt(TIME_ZONE_ID_BOOT_KEY, 0);
    preference_manager.getString(TIME_ZONE_NAME_BOOT_KEY, "").toCharArray(user_preference.time_zone_name, 100, 0);
    preference_manager.getString(TIME_ZONE_CODE_BOOT_KEY, "").toCharArray(user_preference.time_zone_code, 100, 0);
}
user_preference_t *user_preference_get_preference() { return &user_preference; }
Preferences *user_preference_get_manager() { return &preference_manager; }
#endif