
#ifndef _included_display_config
#define _included_display_config
/**
 * dependensi TFT, touchscreen, grafis
 */
#include <lvgl.h>
#include <TFT_eSPI.h>
#include "machine_manager/TimeManager.h"
#include "machine_manager/InternetManager.h"

enum ViewState
{
    VIEW_STATE_NULL,
    VIEW_STATE_LANDING,
    VIEW_STATE_TERAPI_INFO_PASIEN,
    VIEW_STATE_TERAPI_TERAPI,
    VIEW_STATE_CONFIG_WIFI_LIST_WIFI,
    VIEW_STATE_CONFIG_WIFI_INFO_WIFI,
    VIEW_STATE_CONFIG_WAKTU
};
ViewState view_state;

#define SCREEN_HEIGHT 320
#define SCREEN_WIDTH 240
TFT_eSPI myTft = TFT_eSPI(SCREEN_WIDTH, SCREEN_HEIGHT);

static lv_disp_draw_buf_t draw_buffer;
static lv_color_t buffer[SCREEN_HEIGHT * 10];

static const lv_font_t *font_large;
static const lv_font_t *font_normal;
static lv_style_t style_text_muted;
static lv_style_t style_title;
static lv_style_t style_icon;
static lv_style_t style_bullet;

lv_obj_t *_screen_content, *_status_bar, *_screen_overlay, *_message_box;

// GLOBAL COMPONENTS AND FUNCTIONS
// status bar
lv_obj_t *status_bar_wifi_name;
lv_obj_t *status_bar_wifi_symbol;
lv_obj_t *status_bar_wifi_ip_address;
lv_obj_t *status_bar_date_time;
TaskHandle_t global_display_updater_handler = NULL;
String status_bar_date_time_text = "                    ",
       status_bar_wifi_name_text = "                                       ",
       status_bar_ip_address_text = "                                       ",
       view_var_calory = "                ",
       view_var_heartbeat = "                ",
       view_var_o2 = "                ",
       view_var_rotation = "                ",
       view_var_duration = "                ",
       info_status_label_txt = "                ",
       tile_btn_label_txt = "                ";

// LANDING
void view_launch_landing();
void view_launch_landing(uint8_t tab_idx);
void cb_landing_on_mulai_terapi(lv_event_t *event);
void cb_landing_on_config_wifi(lv_event_t *event);
void cb_landing_on_config_time(lv_event_t *event);

// INFO PASIEN
void view_launch_info_pasien();
void cb_info_pasien_on_back(lv_event_t *event);
void cb_info_pasien_on_close(lv_event_t *event);
void cb_info_pasien_data_ok(lv_event_t *event);

// TERAPI
lv_obj_t *info_calory_label;
lv_obj_t *info_oxygen_label;
lv_obj_t *info_rotation_label;
lv_obj_t *info_heart_label;
lv_obj_t *info_time_label;
lv_obj_t *info_status_label;
lv_obj_t *tile_info_status;
lv_obj_t *tile_info_button;
lv_obj_t *tile_info_btn_label;
TaskHandle_t view_updater_terapi_handler = NULL;
void view_launch_terapi();
void cb_terapi_on_btn_press(lv_event_t *event);
void task_view_updater_terapi(void *pvParameters);

// CONFIG WIFI
// views
void view_launch_config_wifi();
void view_launch_config_wifi_connect();
void view_update_display_scan_result();
// callbacks
void cb_config_wifi_on_ssid_selected(lv_event_t *event);
void cb_config_wifi_on_close(lv_event_t *event);
void cb_config_wifi_connect_wifi(lv_event_t *event);
void cb_config_wifi_connect_wifi_on_back(lv_event_t *event);
void cb_config_wifi_password_show_toggle(lv_event_t *event);
// tasks
void task_scan_wifi(void *pvParameters);
void task_config_wifi_wait_connecting(void *pvParameters);

bool wifi_password_is_show = false;
lv_obj_t *list_wifi_container;
static lv_coord_t wifi_opt_col[] = {LV_GRID_FR(1), 50, LV_GRID_TEMPLATE_LAST};
static lv_coord_t wifi_opt_row[] = {LV_GRID_FR(3), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
TaskHandle_t task_after_wifi_connect_attempt_connected_handler,
    task_after_wifi_connect_attempt_disconnected_handler,
    task_wifi_config_scan_async_handler;

// CONFIG WAKTU
lv_obj_t *time_detail_time;
TaskHandle_t task_config_waktu_display_updater;
void view_launch_config_waktu();
void cb_config_waktu_on_sync(lv_event_t *event);
void cb_config_waktu_on_close(lv_event_t *event);
void task_config_waktu_wait_syncing(void *pvParameters);
void task_view_updater_config_waktu(void *pvParameters);

// my functions
void view_status_bar_launch();
void global_display_updater(void *pvParameters);
void message_box_launch(String message, bool with_spinner);
void message_box_clear();
void screen_content_clear();
void text_area_callback(lv_event_t *event);

// lvgl functions
void my_display_flush(lv_disp_drv_t *display, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    myTft.startWrite();
    myTft.setAddrWindow(area->x1, area->y1, w, h);
    myTft.pushColors((uint16_t *)&color_p->full, w * h, true);
    myTft.endWrite();

    lv_disp_flush_ready(display);
}

void my_touchpad_reader(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX, touchY;

    bool touched = myTft.getTouch(&touchX, &touchY, 600);

    if (!touched)
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

void my_log_cb(const char *buf)
{
    Serial.println(String(buf));
}

void display_init()
{
    uint16_t calibrationData[5] = {422, 3505, 253, 3557, 1};
#if LV_USE_LOG
// lv_log_register_print_cb(my_log_cb);
#endif

    lv_init();
    myTft.begin();
    myTft.setRotation(3);
    myTft.setTouch(calibrationData);

    // display graphic init
    lv_disp_draw_buf_init(&draw_buffer, buffer, NULL, SCREEN_HEIGHT * 10);
    static lv_disp_drv_t display_graphic_driver;
    lv_disp_drv_init(&display_graphic_driver);

    // the screen
    display_graphic_driver.hor_res = SCREEN_HEIGHT;
    display_graphic_driver.ver_res = SCREEN_WIDTH;
    display_graphic_driver.flush_cb = my_display_flush;
    display_graphic_driver.draw_buf = &draw_buffer;
    lv_disp_drv_register(&display_graphic_driver);

    // the input
    static lv_indev_drv_t touch_driver;
    lv_indev_drv_init(&touch_driver);
    touch_driver.type = LV_INDEV_TYPE_POINTER;
    touch_driver.read_cb = my_touchpad_reader;
    lv_indev_drv_register(&touch_driver);

    font_normal = &lv_font_montserrat_14;
    // default themeW
    lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE_GREY),
                          LV_THEME_DEFAULT_DARK, font_normal);

    lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_column(lv_scr_act(), 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(lv_scr_act(), 0, LV_STATE_DEFAULT);

    view_status_bar_launch();
}

// MESSAGE BOX
TaskHandle_t message_box_clear_handle;
void screen_overlay_cb(lv_event_t *event)
{
    message_box_clear();
}

void message_box_launch(String message, bool with_spinner)
{
    message_box_clear();

    _screen_overlay = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(_screen_overlay);
    lv_obj_set_style_bg_color(_screen_overlay, lv_color_hex(0X000), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(_screen_overlay, 100, LV_STATE_DEFAULT);
    lv_obj_set_pos(_screen_overlay, 0, 0);
    lv_obj_set_size(_screen_overlay, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_flag(_screen_overlay, LV_OBJ_FLAG_FLOATING);

    static const char *btns[] = {""};
    _message_box = lv_msgbox_create(_screen_overlay, "", message.c_str(), btns, false);
    lv_obj_center(_message_box);

    if (with_spinner)
    {
        // lv_obj_t *box_content = lv_obj_get_parent(lv_msgbox_get_content(_message_box));
        // lv_obj_t *spinner = lv_spinner_create(box_content, 1000, 60);
        // lv_obj_set_align(spinner, LV_ALIGN_BOTTOM_MID);
        // lv_obj_set_size(spinner, 30, 30);
    }

    lv_obj_add_event_cb(_screen_overlay, screen_overlay_cb, LV_EVENT_CLICKED, NULL);
}

void message_box_clear()
{
#if DISPLAY_DEBUGGER
    Serial.println("message box clear");
#endif
    if (lv_obj_is_valid(_screen_overlay))
    {
        lv_obj_clean(_screen_overlay);
        lv_obj_del(_screen_overlay);
    }
    if (lv_obj_is_valid(_message_box))
    {
        lv_obj_clean(_message_box);
        lv_obj_del(_message_box);
    }
}

// STATUS BAR
#define STATUS_BAR_HEIGHT 20

/**
 * SECTION VIEW_STATUS BAR
 */
void view_status_bar_launch()
{
    _status_bar = lv_obj_create(lv_scr_act());

    lv_obj_set_size(_status_bar, LV_PCT(100), STATUS_BAR_HEIGHT);
    lv_obj_set_flex_flow(_status_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(_status_bar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_set_x(_status_bar, 0);
    lv_obj_set_y(_status_bar, 0);
    lv_obj_set_style_pad_ver(_status_bar, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_hor(_status_bar, 4, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(_status_bar, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(_status_bar, 0, LV_STATE_DEFAULT);

    status_bar_date_time = lv_label_create(_status_bar);
    lv_obj_set_style_text_font(status_bar_date_time, &lv_font_montserrat_10, LV_STATE_DEFAULT);
    lv_label_set_text_static(status_bar_date_time, status_bar_date_time_text.c_str());

    // NETWORK
    lv_obj_t *status_bar_network_sect = lv_obj_create(_status_bar);
    lv_obj_set_size(status_bar_network_sect, 200, STATUS_BAR_HEIGHT);
    lv_obj_set_flex_flow(status_bar_network_sect, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(status_bar_network_sect, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_set_x(status_bar_network_sect, 0);
    lv_obj_set_y(status_bar_network_sect, 0);
    lv_obj_set_style_pad_all(status_bar_network_sect, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(status_bar_network_sect, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(status_bar_network_sect, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(status_bar_network_sect, 0, LV_STATE_DEFAULT);

    status_bar_wifi_ip_address = lv_label_create(status_bar_network_sect);
    lv_obj_set_style_text_font(status_bar_wifi_ip_address, &lv_font_montserrat_10, LV_STATE_DEFAULT);
    lv_label_set_text_static(status_bar_wifi_ip_address, status_bar_ip_address_text.c_str());

    status_bar_wifi_name = lv_label_create(status_bar_network_sect);
    lv_obj_set_style_text_font(status_bar_wifi_name, &lv_font_montserrat_10, LV_STATE_DEFAULT);
    lv_label_set_text_static(status_bar_wifi_name, status_bar_wifi_name_text.c_str());

    status_bar_wifi_symbol = lv_label_create(status_bar_network_sect);
    lv_obj_set_style_text_font(status_bar_wifi_symbol, &lv_font_montserrat_10, LV_STATE_DEFAULT);
    lv_label_set_text(status_bar_wifi_symbol, LV_SYMBOL_WIFI);

    xTaskCreatePinnedToCore(
        global_display_updater,
        "stbar_dttm_updt",
        1023 * 3,
        NULL,
        2,
        &global_display_updater_handler,
        ARDUINO_RUNNING_CORE);
}

void view_terapi_disp_updater();

long date_time_last_update = 0,
     wifi_status_last_update = 0;
void global_display_updater(void *pvParameters)
{
    lv_color_t status_wifi_symbol_color,
        status_bar_background = lv_color_hex(0xD6D6D7);

    for (;;)
    {
        if (millis() - date_time_last_update > 200)
        {
            status_bar_date_time_text = String(time_get_hour()) + ":";
            status_bar_date_time_text += String(time_get_minute()) + " ";
            status_bar_date_time_text += String(time_get_date()) + "/";
            status_bar_date_time_text += String(time_get_month()) + "/";
            status_bar_date_time_text += String(time_get_year());
            if (lv_obj_is_valid(status_bar_date_time))
                lv_label_set_text_static(status_bar_date_time, NULL);
            date_time_last_update = millis();
        }

        if (millis() - wifi_status_last_update > 200)
        {
            status_bar_wifi_name_text = "";
            status_bar_ip_address_text = "";
            if (WiFi.status() == WL_CONNECTED)
            {
                status_bar_background = lv_color_hex(0x49F479);
                status_wifi_symbol_color = lv_color_hex(0x444);
                status_bar_wifi_name_text = wifi_get_ssid();
                status_bar_ip_address_text = wifi_get_ip();
                // lv_label_set_text_fmt(status_bar_wifi_name, "%s", wifi_get_ssid());
                // lv_label_set_text_fmt(status_bar_wifi_ip_address, "%s", wifi_get_ip());
            }
            if (WiFi.status() != WL_CONNECTED)
            {
                status_bar_background = lv_color_hex(0xD6D6D7);
                status_wifi_symbol_color = lv_color_hex(0xFF5E48);
                status_bar_wifi_name_text = "Tidak terhubung";
                status_bar_ip_address_text = "-";
            }
            if (internet_is_connected)
            {
                status_wifi_symbol_color = lv_color_hex(0x444);
            }
            else
            {
                status_wifi_symbol_color = lv_color_hex(0xFF5E48);
            }
            // UPDATE
            if (lv_obj_is_valid(status_bar_wifi_name))
                lv_label_set_text_static(status_bar_wifi_name, NULL);
            if (lv_obj_is_valid(status_bar_wifi_ip_address))
                lv_label_set_text_static(status_bar_wifi_ip_address, NULL);
            if (lv_obj_is_valid(_status_bar))
                lv_obj_set_style_bg_color(_status_bar, status_bar_background, LV_STATE_DEFAULT);
            if (lv_obj_is_valid(status_bar_wifi_symbol))
                lv_obj_set_style_text_color(status_bar_wifi_symbol, status_wifi_symbol_color, LV_STATE_DEFAULT);
            wifi_status_last_update = millis();
        }

        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void screen_content_clear()
{
    if (lv_obj_is_valid(_screen_content))
    {
        lv_obj_add_flag(_screen_content, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clean(_screen_content);
        lv_obj_del(_screen_content);
    }
#if DISPLAY_DEBUGGER
    Serial.println("screen_content_clear");
#endif
}

void text_area_callback(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *target = lv_event_get_target(event);
    lv_obj_t *keyboard = (lv_obj_t *)lv_event_get_user_data(event);

    if (code == LV_EVENT_FOCUSED)
    {

#if DISPLAY_DEBUGGER
        Serial.println("focused");
        Serial.println(lv_indev_get_type(lv_indev_get_act()));
#endif
        if (lv_indev_get_type(lv_indev_get_act()) != LV_INDEV_TYPE_KEYPAD)
        {

#if DISPLAY_DEBUGGER
            Serial.println("show");
#endif
            lv_keyboard_set_textarea(keyboard, target);
            lv_obj_update_layout(_screen_content);
            lv_obj_set_height(_screen_content,
                              LV_VER_RES - lv_obj_get_height(keyboard) - STATUS_BAR_HEIGHT);
            lv_obj_scroll_to_view_recursive(target, LV_ANIM_OFF);
            lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
        }
    }
    else if (code == LV_EVENT_DEFOCUSED)
    {
#if DISPLAY_DEBUGGER
        Serial.println("defocused");
#endif

        lv_keyboard_set_textarea(keyboard, NULL);
        lv_obj_update_layout(_screen_content);
        lv_obj_set_height(_screen_content, LV_VER_RES - STATUS_BAR_HEIGHT);
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
        lv_indev_reset(NULL, target);
    }
    else if (code == LV_EVENT_READY || code == LV_EVENT_CANCEL)
    {

#if DISPLAY_DEBUGGER
        Serial.println("ready/cancel");
#endif

        lv_obj_update_layout(_screen_content);
        lv_obj_set_height(_screen_content, LV_VER_RES - STATUS_BAR_HEIGHT);
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(target, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, target);
    }
}

#endif