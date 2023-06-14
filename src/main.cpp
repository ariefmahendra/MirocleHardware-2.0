#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_I2CDevice.h>
#include "WiFi.h"
#include <ArduinoJson.h>
#include "machine_manager/InternetManager.h"
#include "machine_manager/PreferenceManager.h"
#include "machine_manager/TimeManager.h"
#include "machine_manager/MqtManager.h"
#include "machine_manager/PasienManager.h"
#include "machine_manager/TerapiManager.h"
#include "machine_manager/MaxSensorManager.h"
#include "machine_peripheral/Buzzer.h"
#include "display_config.h"
#include "views/view_landing.h"
#include "views/view_info_pasien.h"
#include "views/view_mulai_terapi.h"
#include "views/view_config_wifi.h"
#include "views/view_config_waktu.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

Buzzer buzzer = Buzzer(GPIO_NUM_13, false, 500);
void mq_callback(String &topic, String &payload);
void machine_runner_task(void *pvParameters);
void max_runner_task(void *pvParameters);
void terapi_runner_task(void *pvParameters);

void setup()
{
    Serial.begin(115200);
    buzzer.setDebug(false);

    // inisialisasi user preference
    user_preference_init();

    // inisialisasi wifi & internet
    wifi_init(user_preference_get_preference(), user_preference_get_manager());

    // inisialisasi waktu
    time_init(user_preference_get_preference(), user_preference_get_manager());

    // inisialisasi mqtt
    mq_init();

    // inisialisasi manager pasien
    pasien_manager_init(mq_get_client());

    // menjalankan funsi background mesin
    xTaskCreatePinnedToCore(
        machine_runner_task,
        "machine_runner",
        1023 * 6,
        NULL,
        2,
        NULL,
        1);

    // menjalankan fungsi background sensor max
    xTaskCreatePinnedToCore(
        max_runner_task,
        "max_run",
        1023 * 4,
        NULL,
        1,
        NULL,
        0);

    // menjalankan fungsi bacground terapi
    xTaskCreatePinnedToCore(
        terapi_runner_task,
        "terapi_run",
        1023 * 5,
        NULL,
        1,
        NULL,
        1);

    display_init();
    view_launch_landing();
    rotation_counter.reset_counter();
    rotation_counter.start_counter();
}

long lastTaskReport = 0;
char taskReportPayload[1000];
void loop()
{
    lv_task_handler();

    if (millis() - lastTaskReport > 1000)
    {
        rotation_counter.getValidLinearSpeedMps();
        lastTaskReport = millis();
    }
}

void machine_runner_task(void *pvParameters)
{
    for (;;)
    {
        rotation_counter.runner();
        buzzer.runner();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void max_runner_task(void *pvParameters)
{
    for (;;)
    {
        max_runner();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
void terapi_runner_task(void *pvParameters)
{
    for (;;)
    {
        terapi_runner();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
// LANDING PAGE - CALLBACKS
void cb_landing_on_mulai_terapi(lv_event_t *event)
{
    message_box_launch("Mengambil data pasien", true);
    if (!terapi_init(&buzzer))
    {
        message_box_launch("Tidak dapat memulai terapi", false);
        return;
    }

    message_box_clear();
    screen_content_clear();
    view_launch_info_pasien();
}

void cb_landing_on_config_wifi(lv_event_t *event)
{
    view_launch_config_wifi();
    xTaskCreatePinnedToCore(
        task_scan_wifi,
        "wifi_scan_async",
        1024 * 15,
        NULL,
        2,
        NULL,
        1);
    view_state = VIEW_STATE_CONFIG_WIFI_LIST_WIFI;
}

void cb_landing_on_config_time(lv_event_t *event)
{
    view_launch_config_waktu();
    xTaskCreatePinnedToCore(
        task_view_updater_config_waktu,
        "conf_waktu_up",
        1023 * 2,
        NULL,
        2,
        &task_config_waktu_display_updater,
        1);
}

// INFO PASIEN - CALLBACKS
void cb_info_pasien_on_back(lv_event_t *event)
{
    screen_content_clear();
    view_launch_landing();
}

void cb_info_pasien_on_close(lv_event_t *event)
{
    screen_content_clear();
    view_launch_landing();
}

void cb_info_pasien_data_ok(lv_event_t *event)
{
    view_launch_terapi();

    // screen display updater
    xTaskCreatePinnedToCore(
        task_view_updater_terapi,
        "terapi_updater",
        1023 * 4,
        NULL,
        2,
        &view_updater_terapi_handler,
        1);
    view_state = VIEW_STATE_TERAPI_TERAPI;
}

// TERAPI - VIEWS
String seconds_format(unsigned int secs);

void task_view_updater_terapi(void *pvParameters)
{
    long terapi_last_update = 0;
    lv_color_t status_color;
    for (;;)
    {
        // UPDATE DATA
        if (data_terapi.terapi_param_state == TERAPI_READY)
        {
            view_var_calory = "-";
            view_var_heartbeat = "- bpm";
            view_var_o2 = "-";
            view_var_rotation = "-";
            view_var_duration = "-";
            tile_btn_label_txt = "START";
            info_status_label_txt = "RDY";
            status_color = lv_color_hex(0x26D0FC);
        }
        if (data_terapi.terapi_param_state == TERAPI_RUNNING)
        {
            view_var_calory = String(terapi_get_calory());
            view_var_heartbeat = String(terapi_get_heartbeat());
            view_var_o2 = String(terapi_get_o2());
            view_var_rotation = String(terapi_get_rotation());
            view_var_duration = seconds_format(terapi_get_duration());
            tile_btn_label_txt = "STOP";
            info_status_label_txt = "RUN";
            if ((millis() / 500) % 2 == 0)
            {
                status_color = lv_color_hex(0XFCCD26);
            }
            else
            {
                status_color = lv_color_hex(0x26D0FC);
            }
        }
        if (data_terapi.terapi_param_state == TERAPI_FINISHED)
        {
            tile_btn_label_txt = "EXIT";
            info_status_label_txt = "FINISH";
            status_color = lv_color_hex(0x26FC2E);
        }
        // if (data_terapi.terapi_param_state == TERAPI_STOP)
        // {
        //     tile_btn_label_txt = "STOPPED";
        //     if (lv_obj_is_valid(tile_info_status))
        //         lv_obj_set_style_bg_color(tile_info_status, lv_color_hex(0xFF5E48), LV_STATE_DEFAULT);
        // }

        // UPDATE DISPLAY
        if (terapi_get_state() != TERAPI_INIT)
        {
            if (lv_obj_is_valid(tile_info_status))
                lv_obj_set_style_bg_color(tile_info_status, status_color, LV_STATE_DEFAULT);

            if (lv_obj_is_valid(info_calory_label))
                lv_label_set_text_static(info_calory_label, NULL);

            if (lv_obj_is_valid(info_heart_label))
                lv_label_set_text_static(info_heart_label, NULL);

            if (lv_obj_is_valid(info_oxygen_label))
                lv_label_set_text_static(info_oxygen_label, NULL);

            if (lv_obj_is_valid(info_rotation_label))
                lv_label_set_text_static(info_rotation_label, NULL);

            if (lv_obj_is_valid(info_time_label))
                lv_label_set_text_static(info_time_label, NULL);

            if (lv_obj_is_valid(tile_info_btn_label))
                lv_label_set_text_static(tile_info_btn_label, NULL);
            // lv_label_set_text(tile_info_btn_label, "STOP");

            if (lv_obj_is_valid(info_status_label))
                lv_label_set_text_static(info_status_label, NULL);
            // lv_label_set_text(info_status_label, "RUN");
            // terapi_last_update = millis();
        }

        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

// TERAPI - CALLBACKS
void cb_terapi_on_btn_press(lv_event_t *event)
{
    switch (terapi_get_state())
    {
    case TERAPI_INIT:
        break;
    case TERAPI_READY:
        // register data harvester task
        terapi_start();
        break;
    case TERAPI_RUNNING:
        // unregister data harvester task
        terapi_stop();
        break;
    case TERAPI_FINISHED:
        // close window, jump to result
        terapi_reset();
        view_launch_landing();
        break;
    default:
        break;
    }
}

// CONFIG WIFI - CALLBACKS
void cb_config_wifi_on_ssid_selected(lv_event_t *event)
{
    wifi_t *wifi_selected = (wifi_t *)lv_event_get_user_data(event);
    Serial.printf("Wifi selected: %s| id: %d", wifi_selected->ssid, wifi_selected->id);
    wifi_set_selected_id(wifi_selected->id);
    view_launch_config_wifi_connect();
}

TaskHandle_t task_handle_wifi_scan;
void cb_config_wifi_on_close(lv_event_t *event)
{
    wifi_set_selected_id(-1);
    screen_content_clear();
    view_launch_landing(1);
}

void cb_config_wifi_connect_wifi(lv_event_t *event)
{
    if (wifi_is_connecting)
        return;
    lv_obj_t *wifi_password_input = (lv_obj_t *)lv_event_get_user_data(event);
    String a = "";
    if (wifi_password_input != NULL)
        a = lv_textarea_get_text(wifi_password_input);
    wifi_set_key(a);
    wifi_is_connecting = true;
    xTaskCreatePinnedToCore(
        task_config_wifi_wait_connecting,
        "waiting_wifi",
        1023 * 20,
        NULL,
        2,
        NULL,
        1);
}

void cb_config_wifi_connect_wifi_on_back(lv_event_t *event)
{
    view_launch_config_wifi();
    xTaskCreatePinnedToCore(
        task_scan_wifi,
        "wifi_scan_async",
        1024 * 15,
        NULL,
        2,
        &task_handle_wifi_scan,
        1);
    view_state = VIEW_STATE_CONFIG_WIFI_LIST_WIFI;
}

void cb_config_wifi_password_show_toggle(lv_event_t *event)
{
    lv_obj_t *toggle_btn = lv_event_get_target(event);
    lv_obj_t *wifi_password_input = (lv_obj_t *)lv_event_get_user_data(event);

    wifi_password_is_show = !wifi_password_is_show;

    if (wifi_password_is_show)
    {
        lv_label_set_text(toggle_btn, LV_SYMBOL_EYE_OPEN);
        lv_textarea_set_password_mode(wifi_password_input, false);
    }
    else
    {
        lv_label_set_text(toggle_btn, LV_SYMBOL_EYE_CLOSE);
        lv_textarea_set_password_mode(wifi_password_input, true);
    }
}

// CONFIG WIFI - TASKS
void task_scan_wifi(void *pvParameters)
{
    Serial.println("task scan wifi");
    if (WiFi.scanComplete() == WIFI_SCAN_RUNNING)
        vTaskDelete(NULL);

    message_box_launch("Mencari wifi...", true);
    wifi_scan();
    Serial.println(WiFi.scanComplete());
    if (wifi_ssid_count <= 0)
    {
        message_box_launch("Gagal mencari wifi!", false);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        message_box_clear();
    }
    else if (wifi_ssid_count > 0)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        message_box_clear();
        vTaskDelay(100 / portTICK_PERIOD_MS);
        message_box_clear();
        view_update_display_scan_result();
    }

    vTaskDelete(NULL);
}

void task_config_wifi_wait_connecting(void *pvParameters)
{
    message_box_launch("Menghubungkan wifi", true);
    wifi_connect();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    wifi_is_connecting = false;
    switch (WiFi.status())
    {
    case WL_CONNECTED:
        message_box_launch("Wifi tersambung", false);
        vTaskDelete(NULL);
        break;

    case WL_CONNECT_FAILED:
        message_box_launch("Gagal menyambungkan wifi", false);
        vTaskDelete(NULL);
        break;
    default:
        vTaskDelete(NULL);
        break;
    }
}

// CONFIG WAKTU - VIEWS
void task_view_updater_config_waktu(void *pvParameters)
{
    for (;;)
    {
        if (lv_obj_is_valid(time_detail_time))
            lv_label_set_text_fmt(time_detail_time, "%s, %d %s %d %d:%d:%d",
                                  time_get_hari(),
                                  time_get_date(),
                                  time_get_bulan(),
                                  time_get_year(),
                                  time_get_hour(),
                                  time_get_minute(),
                                  time_get_second());
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

// CONFIG WAKTU - CALLBACKS

void cb_config_waktu_on_sync(lv_event_t *event)
{
    if (time_state == TIME_STATE_SYNCING)
        return;
    past_zone_id = time_get_zone_id();
    lv_obj_t *dropdown = (lv_obj_t *)lv_event_get_user_data(event);
    if (lv_obj_is_valid(dropdown))
    {
        time_set_zone(lv_dropdown_get_selected(dropdown));
    }
    xTaskCreatePinnedToCore(
        task_config_waktu_wait_syncing,
        "time_sync",
        1023 * 10,
        NULL,
        2,
        NULL,
        1);
}

void cb_config_waktu_on_close(lv_event_t *event)
{
    vTaskDelete(task_config_waktu_display_updater);
    screen_content_clear();
    view_launch_landing(1);
}

// CONFIG WAKTU - TASKS
void task_config_waktu_wait_syncing(void *pvParameters)
{
    if (!internet_is_connected)
    {
        message_box_launch("Tidak ada internet", false);
        vTaskDelete(NULL);
    }
    message_box_launch("Menyinkronkan waktu", true);
    time_state = TIME_STATE_SYNCING;
    time_sync();
#if MACHINEDEBUG
    Serial.println("syncing done");
#endif
    if (time_state == TIME_STATE_SYNC_SUCCESS)
    {
        message_box_launch("Berhasil menyinkronkan", false);
    }
    if (time_state == TIME_STATE_SYNC_FAILED)
    {
        time_set_zone(past_zone_id);
        message_box_launch("Gagal Menyinkronkan", false);
    }
    vTaskDelete(NULL);
}

String seconds_format(unsigned int secs)
{
    unsigned int sec, min, hour;
    String tmp = "";
    hour = secs / 3600;
    min = (secs % 3600) / 60;
    sec = (secs % 60);

    if (secs >= 3600)
        tmp += String(hour) + " h ";
    if (secs >= 60)
        tmp += String(min) + " m ";
    tmp += String(sec) + " s";

    return tmp;
}