#ifndef _included_terapi_manager
#define _included_terapi_manager
#include "ArduinoJson.h"
#include "machine_manager/MqtManager.h"
#include "machine_manager/PasienManager.h"
#include "machine_manager/TimeManager.h"
#include "machine_manager/MaxSensorManager.h"
#include "machine_peripheral/Buzzer.h"
#include "machine_peripheral/RotationCounter.h"
#include "data/data_terapi.h"
#include "config.h"

data_terapi_t data_terapi;
RotationCounter rotation_counter = RotationCounter(GPIO_NUM_14, GPIO_NUM_27, MODE_ACTIVE_HIGH, 3, 8.5 / 100);
Buzzer *_buzzer_terapi;

static float MET = 4.0;
long calory_update_timer = 0;

TaskHandle_t handler_data_sender_task = NULL;
void terapi_calculate();
float terapi_get_calory();
float terapi_get_o2();
uint terapi_get_rotation();
uint32_t terapi_get_heartbeat();
unsigned int terapi_get_duration();
uint8_t terapi_get_state();
String terapi_get_id();
void terapi_sender_task(void *pvParameters);
void pasien_fetch_task(void *pvParameters);

bool terapi_init(Buzzer *_buzzer)
{
    _buzzer_terapi = _buzzer;
    rotation_counter.setDebug(false);
    if (!max_init())
        return false;

    if (!mq_is_connected())
    {
        return false;
    }

    // terapi
    // data_terapi.terapi_param_state = TERAPI_INIT;
    calory_update_timer = 0;
    data_terapi.terapi_param_calory = 0;
    data_terapi.terapi_param_heartrate = 0;
    data_terapi.terapi_param_heart_count = 0;
    data_terapi.terapi_param_o2 = 0;
    data_terapi.terapi_param_rotation = 0;

    // attempt fetch patient info
    xTaskCreatePinnedToCore(
        pasien_fetch_task,
        "fetchpasien",
        1023 * 3,
        NULL,
        2,
        NULL,
        1);
#if _TERAPI_MANAGER_DEBUG
    Serial.print("\nWaiting pasien data ");
#endif
    while (pasien_info_fetch_state == FETCH_WAIT || pasien_info_fetch_state == FETCH_IDLE)
    {
#if _TERAPI_MANAGER_DEBUG
        Serial.print(".");
#endif
        delay(1000);
    }
    Serial.print("\n");
    if (pasien_info_fetch_state != FETCH_SETTLED)
    {
#if _TERAPI_MANAGER_DEBUG
        Serial.println("terapi cannot init: cannot get pasien info ");
#endif
        return false;
    }

    data_terapi.terapi_param_state = TERAPI_READY;
    return true;
}

void terapi_start()
{
    if (data_terapi.terapi_param_state != TERAPI_READY)
        return;

    // routines
    // calory

    // o2
    // heartbeat
    max_stop_read();
    max_start_read();

    // rotation
    rotation_counter.start_counter();

    // time start
    data_terapi.time_start = time_get_now().unixtime();

    // data sender
    String _topic = "terapi_begin";
    DynamicJsonDocument doc(128 * 4);
    doc[MQ_FIELD_SERI_ALAT] = DEVICE_ID;
    doc[MQ_FIELD_ID_TERAPI] = data_terapi.time_start;
    String _data = "";
    serializeJson(doc, _data);
    mq_send_data(_topic, _data);

    xTaskCreatePinnedToCore(
        terapi_sender_task,
        "data_sender",
        1023 * 4,
        NULL,
        3,
        &handler_data_sender_task,
        0);

    // state
    data_terapi.terapi_param_state = TERAPI_RUNNING;
    // beep
    _buzzer_terapi->beep();
}

void terapi_stop()
{
    if (data_terapi.terapi_param_state != TERAPI_RUNNING)
        return;

    // beep
    _buzzer_terapi->beep();

    // state
    data_terapi.terapi_param_state = TERAPI_FINISHED;

    // routines
    // calory

    // o2
    // heartbeat
    max_stop_read();

    // rotation
    rotation_counter.stop_counter();

    // time end
    data_terapi.time_end = time_get_now().unixtime();

    // SEND FINAL DATA
    String _topic = MQ_COMMAND_STOP;
    DynamicJsonDocument doc(128 * 4);
    doc[MQ_FIELD_ID_TERAPI] = data_terapi.time_start;
    doc[MQ_FIELD_START_TIME] = data_terapi.time_start;
    doc[MQ_FIELD_END_TIME] = data_terapi.time_end;
    doc[MQ_FIELD_DURATION] = terapi_get_duration();
    doc[MQ_FIELD_HEART_RATE_AVERAGE] = (data_terapi.terapi_param_heart_count * 60) / terapi_get_duration();
    doc[MQ_FIELD_CALORY_TOTAL] = data_terapi.terapi_param_calory;
    doc[MQ_FIELD_ROTATION_COUNT] = data_terapi.terapi_param_rotation;
    doc[MQ_FIELD_HEART_RATE] = data_terapi.terapi_param_heartrate;
    doc[MQ_FIELD_O2_SATURATION] = data_terapi.terapi_param_o2;
    String _data = "";
    serializeJson(doc, _data);
    mq_send_data(_topic, _data);

    // data sender
    vTaskDelete(handler_data_sender_task);
}

long beep_timer = 0;
void terapi_runner()
{
    if (data_terapi.terapi_param_state == TERAPI_RUNNING)
    {
        // heartbeat limiter
        if (data_terapi.terapi_param_heartrate >= 120)
        {
            if ((millis() - beep_timer) > 800)
            {
                _buzzer_terapi->beepFor(400);
                beep_timer = millis();
            }
        }
        terapi_calculate();
    }
}

void terapi_calculate()
{
    data_terapi.terapi_param_rotation = rotation_counter.getRotationValid();
    data_terapi.terapi_param_heartrate = max_get_current_bpm();
    data_terapi.terapi_param_heart_count = max_get_beat_count();
    data_terapi.terapi_param_o2 = max_get_spo2();

    if (millis() - calory_update_timer > 2000)
    {
        // calculate ongoing calory
        float linear_speed, met_relative;
        // get rotation speed in m/s
        linear_speed = rotation_counter.getValidLinearSpeedMps();
        // Serial.printf("speed: %f\n", linear_speed);
        // translate rotation speed to km/s
        linear_speed = linear_speed * (3600 / 1000);
        // determine MET from linear speed
        if (linear_speed == 0)
            met_relative = 0;
        else if (linear_speed < 16)
            met_relative = 0.07;
        else if (linear_speed >= 16 && linear_speed <= 19)
            met_relative = 0.10;
        else if (linear_speed > 19 && linear_speed <= 22)
            met_relative = 0.13;
        else if (linear_speed > 22 && linear_speed <= 26)
            met_relative = 0.17;
        else if (linear_speed > 26 && linear_speed <= 30)
            met_relative = 0.20;
        else if (linear_speed > 30)
            met_relative = 0.27;
        data_terapi.terapi_param_calory += (float)((met_relative * 7.7 * pasien.berat_badan) / 200) * 2;
        calory_update_timer = millis();
    }

    // calculate o2
}

float terapi_get_calory()
{
    return data_terapi.terapi_param_calory;
}

float terapi_get_o2()
{
    return data_terapi.terapi_param_o2;
}

uint terapi_get_rotation()
{
    return data_terapi.terapi_param_rotation;
}

uint32_t terapi_get_heartbeat()
{
    return data_terapi.terapi_param_heartrate;
}

unsigned int terapi_get_duration()
{
    if (data_terapi.terapi_param_state == TERAPI_FINISHED)
        return data_terapi.time_end - data_terapi.time_start;
    if (data_terapi.terapi_param_state == TERAPI_RUNNING)
    {
        unsigned int durasi = time_get_now().unixtime() - data_terapi.time_start;
        return durasi;
    }
    return 0;
}

void terapi_reset()
{
    data_terapi.terapi_param_state = TERAPI_INIT;
}

uint8_t terapi_get_state()
{
    return data_terapi.terapi_param_state;
}

String terapi_get_id()
{
    return String(data_terapi.time_start);
}

// TASK
void terapi_sender_task(void *pvParameters)
{
    DynamicJsonDocument doc(128 * 4);
    String payload = "";
    // data_terapi/{terapi_id}
    String topic_data = MQ_COMMAND_DATA_TERAPI + String("/") + terapi_get_id();
    uint data_seq = 0;

    for (;;)
    {
        if (data_terapi.terapi_param_state == TERAPI_RUNNING)
        {
            // doc["dataSequence"] = terapi_data_sequence;
            doc[MQ_FIELD_SEQUENCE_NUM] = data_seq;
            doc[MQ_FIELD_HEART_RATE] = data_terapi.terapi_param_heartrate;
            doc[MQ_FIELD_O2_SATURATION] = data_terapi.terapi_param_o2;
            doc[MQ_FIELD_CALORY] = data_terapi.terapi_param_calory;
            doc[MQ_FIELD_ROTATION_COUNT] = data_terapi.terapi_param_rotation;
            doc[MQ_FIELD_DURATION] = terapi_get_duration();

            serializeJson(doc, payload);

            if (mq_send_data(topic_data, payload))
            {
#if _TERAPI_MANAGER_DEBUG
                Serial.println("sent data: " + payload);
#endif
            }

            payload = "";
            data_seq++;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void pasien_fetch_task(void *pvParameters)
{
    pasien_fetch_info();
    vTaskDelete(NULL);
}

#endif