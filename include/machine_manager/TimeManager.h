#ifndef _included_time_manager
#define _included_time_manager
#include "RTClib.h"
#include "time.h"
#include "sntp.h"
#include "data/user_preference.h"
#include "data/data_time_zone.h"
#include "config.h"
#include "InternetManager.h"
#include "PreferenceManager.h"
#include "Preferences.h"

RTC_DS3231 myRTC;
#define TIME_ZONE_COUNT 4
time_zone_t time_zone_list[TIME_ZONE_COUNT] = {{0, "Asia/Jakarta", "WIB-7"}, {1, "Asia/Jayapura", "WIT-9"}, {2, "Asia/Makassar", "WITA-8"}, {3, "Asia/Pontianak", "WIB-7"}};
const char ntp_server_1[20] = "time.google.com",
           ntp_server_2[20] = "0.id.pool.ntp.org",
           ntp_server_3[20] = "time.nist.gov",
           time_zone_code[30] = "CET-1CEST,M3.5.0,M10.5.0/3",
           days_of_the_week[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"},
           month_of_the_year[12][12] = {"Januari", "Februari", "April", "Mei", "Juni", "Juli", "Agustus", "September", "Oktober", "November", "Desember"};
const uint16_t gmt_offset_sec = 3600,
               daylight_offset_sec = 3600;
uint8_t past_zone_id;
time_state_enum time_state = TIME_STATE_IDLE;
user_preference_t *_time_user_preference = NULL;
Preferences *_time_preference_manager = NULL;

bool time_init(user_preference_t *_u_p, Preferences *_pref_manager)
{
    _time_preference_manager = _pref_manager;
    _time_user_preference = _u_p;
    if (!myRTC.begin())
    {
#if _TIME_MANAGER_DEBUG
        Serial.println("RTC not found");
#endif
        return false;
    }
    configTime(gmt_offset_sec, daylight_offset_sec, ntp_server_1, ntp_server_2, ntp_server_3);
    if (_time_user_preference != NULL)
        configTzTime(_time_user_preference->time_zone_code, ntp_server_1, ntp_server_2, ntp_server_3);
    else
        configTzTime(time_zone_list[0].code, ntp_server_1, ntp_server_2, ntp_server_3);

#if _TIME_MANAGER_DEBUG
    Serial.println("time_manager init success");
#endif
    return true;
}

bool time_sync()
{
    time_state = TIME_STATE_SYNCING;

    if (!internet_is_connected)
    {
        time_state = TIME_STATE_SYNC_FAILED;
        return false;
    }
    configTzTime(_time_user_preference->time_zone_code, ntp_server_1, ntp_server_2, ntp_server_3);

    struct tm time_info;
    if (!getLocalTime(&time_info))
    {
#if _TIME_MANAGER_DEBUG
        Serial.println("Time not available yet");
#endif
        time_state = TIME_STATE_SYNC_FAILED;

        return false;
    }

#if _TIME_MANAGER_DEBUG
    Serial.println("Success get time");
    Serial.println(&time_info, "%A, %B %d %Y %H:%M:%S");
    Serial.printf("TIME: %d %d %d %d %d %d\n",
                  time_info.tm_year,
                  time_info.tm_mon,
                  time_info.tm_mday,
                  time_info.tm_hour,
                  time_info.tm_min,
                  time_info.tm_sec);
#endif

    // year, month, date, hour, minute, sec
    char buffer_date[50];
    char buffer_time[50];
    strftime(buffer_date, 30, "%b %d %Y", &time_info);
    strftime(buffer_time, 20, "%T", &time_info);
    myRTC.adjust(DateTime(buffer_date, buffer_time));

    delay(1000);

#if _TIME_MANAGER_DEBUG
    Serial.println("time_manager sync success");
#endif
    time_state = TIME_STATE_SYNC_SUCCESS;
    return true;
}

bool time_set_zone(uint8_t id)
{
    bool found;
    for (uint8_t i = 0; i < TIME_ZONE_COUNT; i++)
    {
        if (id == time_zone_list[i].id)
        {
            _time_preference_manager->putUInt(TIME_ZONE_ID_BOOT_KEY, time_zone_list[id].id);
            _time_preference_manager->putString(TIME_ZONE_NAME_BOOT_KEY, String(time_zone_list[id].name));
            _time_preference_manager->putString(TIME_ZONE_CODE_BOOT_KEY, String(time_zone_list[id].code));
            user_preference_update();
            configTzTime(_time_user_preference->time_zone_code, ntp_server_1, ntp_server_2, ntp_server_3);
            return true;
        }
    }
    return false;
}

uint8_t time_get_zone_id()
{
    return _time_user_preference->time_zone_id;
}

DateTime time_get_now()
{
    DateTime tmpdatetime = myRTC.now();
    while (!tmpdatetime.isValid())
    {
        tmpdatetime = myRTC.now();
        delay(5);
    }
    return tmpdatetime;
}
uint8_t time_get_date() { return time_get_now().day(); }

uint8_t time_get_month() { return time_get_now().month(); }

uint16_t time_get_year() { return time_get_now().year(); }

uint8_t time_get_hour() { return time_get_now().hour(); }

uint8_t time_get_minute() { return time_get_now().minute(); }

uint8_t time_get_second() { return time_get_now().second(); }

String time_get_timestamp() { return time_get_now().timestamp(); }

String time_get_hari()
{
    return days_of_the_week[myRTC.now().dayOfTheWeek()];
}

String time_get_bulan() { return month_of_the_year[(myRTC.now().month() - 1) % 12]; }

#endif