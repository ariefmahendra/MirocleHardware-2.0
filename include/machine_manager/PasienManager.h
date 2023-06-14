#ifndef _included_pasien_manager
#define _included_pasien_manager
#include <MQTT.h>
#include <ArduinoJson.h>
#include "data/data_pasien.h"
#include "config.h"

uint8_t pasien_info_fetch_state = FETCH_IDLE;
long pasien_info_request_time = 0;
pasien_t pasien;
MQTTClient *_pasien_man_mqt_client = NULL;

bool pasien_fetch_info();
void pasien_set_name(String nama_pasien);
void pasien_set_usia(uint8_t usia);
void pasien_set_jenis_kelamin(bool jenis_kelamin);
void pasien_set_berat_badan(float berat_badan);
String terapi_get_pasien_name();
bool terapi_get_pasien_jenis_kelamin();
uint8_t terapi_get_pasien_usia();
float terapi_get_pasien_berat_badan();
bool pasien_manager_init(MQTTClient *_mqt_client)
{
    _pasien_man_mqt_client = _mqt_client;
    if (_pasien_man_mqt_client == NULL)
        return false;
    return true;
}
bool pasien_fetch_info()
{
    if (pasien_info_fetch_state == FETCH_SETTLED)
        pasien_info_fetch_state = FETCH_IDLE;
    if (pasien_info_fetch_state == FETCH_TIMEOUT)
        pasien_info_fetch_state = FETCH_IDLE;
    if (pasien_info_fetch_state == FETCH_WAIT)
        return false;
    pasien_info_request_time = millis();
    String request_payload = "";
    request_payload += "{";
    request_payload += "\"request_time\": " + String(pasien_info_request_time);
    request_payload += ",\"device_id\": \"" + String(DEVICE_ID) + "\"";
    request_payload += ",\"request_type\": \"" + String("info_pasien") + "\"";
    request_payload += "}";

#if _PASIEN_MANAGER_DEBUG
    Serial.println("data_fetch_patient payload: " + request_payload);
#endif
    _pasien_man_mqt_client->setTimeout(2000);
    if (!_pasien_man_mqt_client->publish(MQ_TOPIC_DATA_REQUEST, request_payload))
    {
#if _PASIEN_MANAGER_DEBUG
        Serial.println("data_fetch_patient failed publishing");
#endif
        return false;
    }
#if _PASIEN_MANAGER_DEBUG
    Serial.println("data_fetch_patient request published");
#endif
    pasien_info_fetch_state = FETCH_WAIT;
#if _PASIEN_MANAGER_DEBUG
    Serial.println("data_fetch_patient waiting reply");
#endif
    int fetch_timeout = 10000;
    while (pasien_info_fetch_state == FETCH_WAIT && fetch_timeout > 0)
    {
#ifdef _PASIEN_MANAGER_DEBUG
        Serial.print(".");
#endif
        delay(500);
        fetch_timeout -= 500;
    }
    if (fetch_timeout <= 0)
    {
#ifdef _PASIEN_MANAGER_DEBUG
        Serial.println("\ndata_fetch_patient timeout");
#endif
        pasien_info_fetch_state = FETCH_TIMEOUT;
        return false;
    }
#ifdef _PASIEN_MANAGER_DEBUG
    Serial.println("data_fetch_patient success getting data");
    Serial.println(pasien.nama);
    Serial.printf("nama: %c, \njenis_kelamin: %s, \nusia: %d\n",
                  pasien.nama,
                  pasien.jenis_kelamin == LAKI_LAKI ? "laki-laki" : "perempuan",
                  pasien.usia);
    Serial.println("data_fetch_patient done");
#endif
    return true;
}

long pasien_fetch_info_request_time()
{
    return pasien_info_request_time;
}
void pasien_set_name(String nama_pasien)
{
    nama_pasien.toCharArray(pasien.nama, 200 + 1, 0);
}
void pasien_set_usia(uint8_t usia)
{
    pasien.usia = usia;
}
void pasien_set_jenis_kelamin(bool jenis_kelamin)
{
    pasien.jenis_kelamin = jenis_kelamin;
}
void pasien_set_berat_badan(float berat_badan)
{
    pasien.berat_badan = berat_badan;
}
String terapi_get_pasien_name() { return pasien.nama; }
bool terapi_get_pasien_jenis_kelamin() { return pasien.jenis_kelamin; }
uint8_t terapi_get_pasien_usia() { return pasien.usia; }
float terapi_get_pasien_berat_badan() { return pasien.berat_badan; }

#endif