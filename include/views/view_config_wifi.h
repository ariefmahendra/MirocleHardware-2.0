#ifndef _include_view_config_wifi
#define _include_view_config_wifi
#include "display_config.h"
#include "machine_manager/InternetManager.h"

/**
 * SECTION CONFIG_WIFI
 */
bool wifi_is_connecting = false;
void view_launch_config_wifi()
{
    screen_content_clear();
    _screen_content = lv_win_create(lv_scr_act(), 40);
    lv_obj_set_height(_screen_content, LV_VER_RES - STATUS_BAR_HEIGHT);

    lv_win_add_title(_screen_content, "Pengaturan Wifi");
    lv_obj_set_style_pad_all(lv_win_get_header(_screen_content), 6, LV_STATE_DEFAULT);
    lv_obj_t *btn_keluar = lv_win_add_btn(_screen_content, LV_SYMBOL_CLOSE, 40);
    lv_obj_add_event_cb(btn_keluar, cb_config_wifi_on_close, LV_EVENT_CLICKED, NULL);

    // list wifi
    list_wifi_container = lv_win_get_content(_screen_content);
    lv_obj_set_size(list_wifi_container, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(list_wifi_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_column(list_wifi_container, 4, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(list_wifi_container, 6, LV_STATE_DEFAULT);
    lv_task_handler();
}

void view_launch_config_wifi_connect()
{
    screen_content_clear();
    _screen_content = lv_win_create(lv_scr_act(), 40);
    lv_obj_set_height(_screen_content, LV_VER_RES - STATUS_BAR_HEIGHT);
    lv_obj_set_style_pad_all(lv_win_get_header(_screen_content), 6, LV_STATE_DEFAULT);

    lv_obj_t *btn_kembali = lv_win_add_btn(_screen_content, LV_SYMBOL_LEFT, 40);
    lv_obj_add_event_cb(btn_kembali, cb_config_wifi_connect_wifi_on_back, LV_EVENT_CLICKED, NULL);
    lv_win_add_title(_screen_content, "Sambungkan WiFi");
    lv_obj_t *btn_keluar = lv_win_add_btn(_screen_content, LV_SYMBOL_CLOSE, 40);
    lv_obj_add_event_cb(btn_keluar, cb_config_wifi_on_close, LV_EVENT_CLICKED, NULL);

    // list wifi
    lv_obj_t *container = lv_win_get_content(_screen_content);
    lv_obj_set_size(container, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_column(container, 4, LV_STATE_DEFAULT);

    lv_obj_t *wifi_opt = lv_obj_create(container);
    lv_obj_set_size(wifi_opt, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(wifi_opt, LV_FLEX_FLOW_COLUMN);

    if (wifi_get_selected() == NULL)
        return;
    lv_obj_t *wifi_opt_ssid = lv_label_create(wifi_opt);
    lv_obj_set_grid_cell(wifi_opt_ssid, LV_GRID_ALIGN_STRETCH, 0, 1,
                         LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_label_set_text_fmt(wifi_opt_ssid, "%s", wifi_get_selected()->ssid);
    lv_obj_set_style_text_font(wifi_opt_ssid, &lv_font_montserrat_18, LV_STATE_DEFAULT);

    lv_obj_t *wifi_opt_desc = lv_label_create(wifi_opt);
    lv_obj_set_grid_cell(wifi_opt_desc, LV_GRID_ALIGN_STRETCH, 0, 1,
                         LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_label_set_text_fmt(wifi_opt_desc, "rssi: %d | %s", wifi_get_selected()->rssi, enctype_type_get(wifi_get_selected()->encryption_type));

    lv_obj_t *wifi_password_input = NULL;
    if (wifi_get_selected()->encryption_type != WIFI_AUTH_OPEN)
    {
        lv_obj_t *wifi_password = lv_obj_create(container);
        lv_obj_set_size(wifi_password, lv_pct(100), LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(wifi_password, LV_FLEX_FLOW_COLUMN);

        lv_obj_t *wifi_password_label = lv_label_create(wifi_password);
        lv_label_set_text(wifi_password_label, "Sandi");

        lv_obj_t *keyboard;
        keyboard = lv_keyboard_create(lv_scr_act());
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

        lv_obj_t *wifi_password_row = lv_obj_create(wifi_password);
        lv_obj_remove_style_all(wifi_password_row);
        lv_obj_set_size(wifi_password_row, lv_pct(100), LV_SIZE_CONTENT);

        static lv_coord_t passrow[] = {LV_GRID_FR(9), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t passcol[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        lv_obj_set_grid_dsc_array(wifi_password_row, passrow, passcol);

        wifi_password_input = lv_textarea_create(wifi_password_row);
        lv_textarea_set_password_mode(wifi_password_input, true);
        lv_textarea_set_one_line(wifi_password_input, true);
        lv_obj_set_width(wifi_password_input, lv_pct(80));
        lv_obj_add_event_cb(wifi_password_input, text_area_callback, LV_EVENT_ALL, keyboard);
        lv_obj_set_grid_cell(wifi_password_input, LV_GRID_ALIGN_STRETCH, 0, 1,
                             LV_GRID_ALIGN_STRETCH, 0, 1);

        lv_obj_t *wifi_password_show_toggle = lv_label_create(wifi_password_row);
        lv_obj_add_flag(wifi_password_show_toggle, LV_OBJ_FLAG_CLICKABLE);
        lv_label_set_text(wifi_password_show_toggle, LV_SYMBOL_EYE_OPEN);
        lv_obj_center(wifi_password_show_toggle);
        lv_obj_set_grid_cell(wifi_password_show_toggle, LV_GRID_ALIGN_CENTER, 1, 1,
                             LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_add_event_cb(wifi_password_show_toggle, cb_config_wifi_password_show_toggle, LV_EVENT_CLICKED, wifi_password_input);
    }

    lv_obj_t *wifi_connect_go = lv_btn_create(container);
    lv_obj_t *wifi_connect_go_label = lv_label_create(wifi_connect_go);
    lv_label_set_text(wifi_connect_go_label, "Sambungkan");
    lv_obj_add_event_cb(wifi_connect_go, cb_config_wifi_connect_wifi, LV_EVENT_CLICKED, wifi_password_input);
    view_state = VIEW_STATE_CONFIG_WIFI_INFO_WIFI;
}

void view_update_display_scan_result()
{
    if (!lv_obj_is_valid(list_wifi_container))
        return;

    for (uint8_t i = 0; i < wifi_ssid_count; i++)
    {
        lv_obj_t *wifi_opt = lv_obj_create(list_wifi_container);

        lv_obj_set_width(wifi_opt, lv_pct(100));
        lv_obj_set_height(wifi_opt, 70);
        lv_obj_add_flag(wifi_opt, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_color(wifi_opt, lv_color_hex(0XD6D6D7), LV_STATE_PRESSED);
        lv_obj_add_event_cb(wifi_opt, cb_config_wifi_on_ssid_selected, LV_EVENT_CLICKED, wifi_list_get(i));

        lv_obj_set_grid_dsc_array(wifi_opt, wifi_opt_col, wifi_opt_row);

        lv_obj_t *wifi_opt_ssid = lv_label_create(wifi_opt);
        lv_obj_set_grid_cell(wifi_opt_ssid, LV_GRID_ALIGN_STRETCH, 0, 1,
                             LV_GRID_ALIGN_STRETCH, 0, 1);
        lv_label_set_text_fmt(wifi_opt_ssid, "%s", wifi_list_get(i)->ssid);
        lv_obj_set_style_text_font(wifi_opt_ssid, &lv_font_montserrat_18, LV_STATE_DEFAULT);

        lv_obj_t *wifi_opt_desc = lv_label_create(wifi_opt);
        lv_obj_set_grid_cell(wifi_opt_desc, LV_GRID_ALIGN_STRETCH, 0, 1,
                             LV_GRID_ALIGN_STRETCH, 1, 1);
        lv_label_set_text_fmt(wifi_opt_desc, "%d | %s", wifi_list_get(i)->rssi, enctype_type_get(wifi_list_get(i)->encryption_type));

        lv_obj_t *wifi_lock = lv_label_create(wifi_opt);
        lv_label_set_text(wifi_lock, LV_SYMBOL_WIFI);
        lv_obj_set_grid_cell(wifi_lock, LV_GRID_ALIGN_STRETCH, 1, 1,
                             LV_GRID_ALIGN_CENTER, 0, 2);
    }
}

#endif