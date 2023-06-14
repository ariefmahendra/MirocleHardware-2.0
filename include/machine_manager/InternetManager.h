#ifndef _included_internet_manager
#define _included_internet_manager
#include "WiFi.h"
#include <ESP32Ping.h>
#include "Preferences.h"
#include "config.h"
#include "data/data_wifi.h"
#include "data/user_preference.h"
#include "data/data_internet_state.h"

uint wifi_bssid, wifi_selected_id;
int8_t wifi_ssid_count;
unsigned long wifi_list_last_update = 0;
bool internet_is_checking = false;
bool internet_is_connected = false;
wifi_t *data_list_wifi = NULL;
user_preference_t *_wifi_user_preference = NULL;
Preferences *_wifi_preference_manager = NULL;

// = INTERNET
#include <ESP32Ping.h>
bool internet_check()
{
    if (internet_is_checking)
        return false;
    internet_is_checking = true;
#if _INTERNET_MANAGER_DEBUG
    Serial.println("Ping google");
#endif
    if (Ping.ping("www.google.com"))
    {
#if _INTERNET_MANAGER_DEBUG
        Serial.println("Internet available");
#endif
        internet_is_connected = true;
    }
    else
    {
        internet_is_connected = false;
    }
    internet_is_checking = false;
    return internet_is_connected;
}
void wifi_runner_task(void *pvParameters)
{
    for (;;)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            internet_check();
        }
        else if (WiFi.status() == WL_DISCONNECTED || WiFi.status() == WL_CONNECTION_LOST || WiFi.status() == WL_IDLE_STATUS)
        {
            internet_is_connected = false;
        }
#if _INTERNET_MANAGER_DEBUG
        Serial.println(WiFi.status());
#endif
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

// = WIFI
void wifi_init(user_preference_t *_u_p, Preferences *_pref_manager)
{
    _wifi_user_preference = _u_p;
    _wifi_preference_manager = _pref_manager;
    WiFi.mode(WIFI_STA);
    WiFi.setTxPower(WIFI_POWER_15dBm);
    WiFi.setAutoReconnect(false);
    if (_wifi_user_preference == NULL)
        return;
    WiFi.begin(_wifi_user_preference->wifi_ssid, _wifi_user_preference->wifi_key);
    xTaskCreatePinnedToCore(
        wifi_runner_task,
        "wifi_runner",
        1023 * 3,
        NULL,
        2,
        NULL,
        0);
    delay(100);
}

uint8_t wifi_scan()
{
    if (wifi_list_last_update != 0)
        if (millis() - wifi_list_last_update < 10000)
        {
            return wifi_ssid_count;
        }

    WiFi.setAutoReconnect(false);
    WiFi.disconnect();
#if _INTERNET_MANAGER_DEBUG
    Serial.println("Scanning WiFi networks");
#endif
    WiFi.scanNetworks();

    vTaskDelay(100 / portTICK_PERIOD_MS);

    if (WiFi.scanComplete() == WIFI_SCAN_FAILED)
    {
        wifi_ssid_count = 0;
        WiFi.setAutoReconnect(true);
        return 0;
    }

    wifi_ssid_count = WiFi.scanComplete();

#if _INTERNET_MANAGER_DEBUG
    Serial.printf("Wifi count: %d\n", wifi_ssid_count);
#endif
    if (wifi_ssid_count > 0)
    {
        free(data_list_wifi);
        data_list_wifi = (wifi_t *)malloc(wifi_ssid_count * sizeof(wifi_t));

        for (uint8_t i = 0; i < wifi_ssid_count; i++)
        {
            data_list_wifi[i].id = i;
            data_list_wifi[i].bssid = *WiFi.BSSID(i);
            WiFi.SSID(i).toCharArray(data_list_wifi[i].ssid, 50);
            data_list_wifi[i].channel = WiFi.channel(i);
            data_list_wifi[i].encryption_type = WiFi.encryptionType(i);
            data_list_wifi[i].rssi = WiFi.RSSI(i);
        }
        wifi_list_last_update = millis();
        WiFi.scanDelete();
    }

    WiFi.setAutoReconnect(true);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    return wifi_ssid_count;
}

uint8_t wifi_get_ssid_count()
{
    return wifi_ssid_count;
}

wifi_t *wifi_list_get(uint8_t i)
{
    if (data_list_wifi == NULL)
        return NULL;
    return &data_list_wifi[i];
}

void wifi_set_selected_id(int8_t id)
{
    if (id == -1)
    {
        WiFi.reconnect();
    }
    delay(10);

#if _INTERNET_MANAGER_DEBUG
    Serial.printf("Selected wifi_id: %d\n", id);
#endif

    wifi_selected_id = id;
}

void wifi_set_key(String key)
{
    if (wifi_selected_id < 0)
        return;

    key.toCharArray(data_list_wifi[wifi_selected_id].key, 50);
}

wifi_t *wifi_get_selected()
{
    if (wifi_selected_id >= 0)
        return &data_list_wifi[wifi_selected_id];
    else
        return NULL;
}

void wifi_connect()
{
#if _INTERNET_MANAGER_DEBUG
    Serial.printf("Connecting wifi, ssid: %s, key: %s\n", data_list_wifi[wifi_selected_id].ssid, data_list_wifi[wifi_selected_id].key);
#endif
    uint8_t state = WiFi.begin(data_list_wifi[wifi_selected_id].ssid, data_list_wifi[wifi_selected_id].key);
    while (1)
    {
        state = WiFi.status();
        if (state == WL_CONNECTED || state == WL_CONNECT_FAILED)
            break;
        vTaskDelay(10 / portTICK_PERIOD_MS);
#if INTERNET_MANAGER_DEBUG
        Serial.println(".");
#endif
    }
    if (state == WL_CONNECTED)
    {
        if (_wifi_preference_manager != NULL)
        {
            _wifi_preference_manager->putString(WIFI_SSID_BOOT_KEY, String(data_list_wifi[wifi_selected_id].ssid));
            _wifi_preference_manager->putString(WIFI_KEY_BOOT_KEY, String(data_list_wifi[wifi_selected_id].key));
        }
    }
#if _INTERNET_MANAGER_DEBUG
    Serial.println(state);
    Serial.println("wifi_connect done");
#endif
    return;
}

void wifi_connect(char ssid[], char key[])
{
#if _INTERNET_MANAGER_DEBUG
    Serial.printf("Connecting wifi, ssid: %s, key: %s\n", String(ssid), String(key));
#endif
    uint8_t state = WiFi.begin(ssid, key);
    while (WiFi.status() != WL_CONNECTED)
    {
#if _INTERNET_MANAGER_DEBUG
        Serial.print(".");
#endif
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);

#if _INTERNET_MANAGER_DEBUG
    Serial.println("\nWiFi Connected");
#endif
    internet_check();
}

void wifi_set_event_cb(WiFiEventCb cb)
{
    WiFi.onEvent(cb);
}

String wifi_get_ssid() { return WiFi.SSID(); }

String wifi_get_ip() { return WiFi.localIP().toString(); }

bool *internet_get_state_handler()
{
    return &internet_is_connected;
}

String enctype_type_get(wifi_auth_mode_t i)
{
    String enctype;
    switch (i)
    {
    case WIFI_AUTH_OPEN:
        enctype = "open";
        break;
    case WIFI_AUTH_WEP:
        enctype = "WEP";
        break;
    case WIFI_AUTH_WPA_PSK:
        enctype = "WPA";
        break;
    case WIFI_AUTH_WPA2_PSK:
        enctype = "WPA2";
        break;
    case WIFI_AUTH_WPA_WPA2_PSK:
        enctype = "WPA+WPA2";
        break;
    case WIFI_AUTH_WPA2_ENTERPRISE:
        enctype = "WPA2-EAP";
        break;
    case WIFI_AUTH_WPA3_PSK:
        enctype = "WPA3";
        break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        enctype = "WPA2+WPA3";
        break;
    case WIFI_AUTH_WAPI_PSK:
        enctype = "WAPI";
        break;
    default:
        enctype = "unknown";
    }

    return enctype;
}
#endif