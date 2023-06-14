#ifndef _included_mqt_manager
#define _included_mqt_manager
#include <MQTT.h>
#include "WiFi.h"
#include "InternetManager.h"
#include "data/data_internet_state.h"
#include "machine_manager/PasienManager.h"
#include "config.h"

WiFiClient wifi_client;
MQTTClient mq_client(MQ_BUFFER_SIZE);

enum mq_state_enum
{
    MQ_UNINITIALIZED,
    MQ_INITIALIZED,
    MQ_NOT_CONNECTED,
    MQ_CONNECTING,
    MQ_CONNECTED
};
mq_state_enum mq_state = MQ_NOT_CONNECTED;

bool mq_init();
bool mq_connect();
bool mq_send_data(String &topic, String &data);

// bool mq_send_data(char topic[], char data[]);
void mq_callback(String &topic, String &payload);
void mq_runner_task(void *pvParameters);

MQTTClient *mq_get_client() { return &mq_client; }
bool mq_init(/*, MQTTClientCallbackSimpleFunction onMessage*/)
{
    mq_client.begin(mq_server, mq_port, wifi_client);
    mq_client.onMessage(mq_callback);
    mq_state = MQ_INITIALIZED;
    xTaskCreatePinnedToCore(
        mq_runner_task,
        "mq_runner",
        1023 * 3,
        NULL,
        2,
        NULL,
        0);
    return true;
}
void mq_subscribe_topic()
{
    mq_client.subscribe(MQ_TOPIC_DATA_REPLY);
}
bool mq_connect()
{
    if (mq_state == MQ_UNINITIALIZED)
        return false;
    if (mq_state == MQ_CONNECTING)
        return false;
    if (!internet_is_connected)
    {
#if _MQT_MANAGER_DEBUG
        Serial.println("MQTT cannot connect, no internet");
#endif
        mq_state = MQ_NOT_CONNECTED;
        return false;
    }
#if _MQT_MANAGER_DEBUG
    Serial.println("MQTT connecting to server");
#endif

    mq_state = MQ_CONNECTING;
    mq_client.setTimeout(4000);
    while (!mq_client.connect(mq_client_id, mq_username, mq_password))
    {
#if _MQT_MANAGER_DEBUG
        Serial.print(".");
#endif
        delay(500);
    }

    if (mq_client.connected())
    {
        mq_state = MQ_CONNECTED;
#if _MQT_MANAGER_DEBUG
        Serial.println("MQTT connected");
#endif
        mq_subscribe_topic();
        return true;
    }
#if _MQT_MANAGER_DEBUG
    Serial.println("MQTT not connected");
    Serial.println("MQTT done connecting");
#endif
    mq_state = MQ_NOT_CONNECTED;
    return false;
}
bool mq_is_connected()
{
    return mq_client.connected();
}
void mq_runner_task(void *pvParameters)
{
    long mq_status_send = 0;
    for (;;)
    {
        // Serial.println("mq loop");
        mq_client.loop();
        if (!mq_client.connected())
        {
            mq_state = MQ_NOT_CONNECTED;
#if _MQT_MANAGER_DEBUG
            Serial.println("mq reconnecting");
#endif
            mq_connect();
        }
        else
        {
            if ((millis() - mq_status_send) > 5000)
            {
                mq_client.publish("data_status", "ok");
                mq_status_send = millis();
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
bool mq_send_data(String &topic, String &data)
{
    if (mq_state == MQ_CONNECTED)
    {
        return mq_client.publish(topic, data);
    }
    return false;
#if _MQT_MANAGER_DEBUG
    Serial.println("MQTT cannot send data, not connected to server");
#endif
}
void mq_callback(String &topic, String &payload)
{
#if _MQT_MANAGER_DEBUG
    Serial.println("MQ got message " + String(topic));
#endif
    DynamicJsonDocument payload_json(128 * 5);
    deserializeJson(payload_json, payload);
    // check device id
    const char *device_id = payload_json["device_id"];
    if (!String(device_id).equals(DEVICE_ID))
    {
        return;
    }
#if _MQT_MANAGER_DEBUG
    Serial.println("Device id match");
#endif
    // check if topic is data reply
    if (topic == MQ_TOPIC_DATA_REPLY)
    {
#if _MQT_MANAGER_DEBUG
        Serial.println("MQ got data reply");
#endif
        // check reply type
        const char *reply_type = payload_json["reply_type"];
        if (String(reply_type) == MQ_FIELD_INFO_PASIEN)
        {
            // check if the request time match the waiting request time
            long request_time = payload_json["request_time"];
            if (request_time == pasien_info_request_time && pasien_info_fetch_state == FETCH_WAIT)
            {
                const char *tmp_name = payload_json["nama"];
                bool jenis_kelamin = payload_json["jenis_kelamin"];
                uint8_t usia = payload_json["usia"];
                float berat_badan = payload_json["berat_badan"];
                pasien_set_name(String(tmp_name));
                pasien_set_jenis_kelamin(jenis_kelamin);
                pasien_set_usia(usia);
                pasien_set_berat_badan(berat_badan);
                pasien_info_request_time = 0;
                pasien_info_fetch_state = FETCH_SETTLED;
            }
        }
    }
}

#endif