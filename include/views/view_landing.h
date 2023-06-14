#ifndef _included_view_landing
#define _included_view_landing
#include <Arduino.h>
#include <lvgl.h>
#include "display_config.h"

// LANDING
void view_launch_landing(uint8_t tab_idx)
{
    view_launch_landing();
    lv_tabview_set_act(_screen_content, tab_idx, LV_ANIM_OFF);
}

void view_launch_landing()
{
    // init view
    screen_content_clear();
    _screen_content = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 40);
    lv_obj_set_height(_screen_content, LV_VER_RES - STATUS_BAR_HEIGHT);

    lv_obj_t *content_menu_home = lv_tabview_add_tab(_screen_content, "Home");
    lv_obj_t *content_menu_settings = lv_tabview_add_tab(_screen_content, "Pengaturan");
    lv_obj_t *content_menu_about = lv_tabview_add_tab(_screen_content, "Tentang");

    // setup menu_home
    lv_obj_set_x(content_menu_home, 0);
    lv_obj_set_y(content_menu_home, 0);

    lv_obj_t *function_list = lv_list_create(content_menu_home);
    lv_obj_set_size(function_list, lv_pct(100), lv_pct(70));

    lv_obj_t *btn_mulai_terapi = lv_list_add_btn(function_list, LV_SYMBOL_RIGHT, "Mulai Terapi");
    lv_obj_add_event_cb(btn_mulai_terapi, cb_landing_on_mulai_terapi, LV_EVENT_CLICKED, NULL);

    // setup menu_setting
    lv_obj_set_x(content_menu_settings, 0);
    lv_obj_set_y(content_menu_settings, 0);

    lv_obj_t *function_list_settings = lv_list_create(content_menu_settings);
    lv_obj_set_size(function_list_settings, lv_pct(100), lv_pct(70));

    lv_obj_t *btn_setting_wifi = lv_list_add_btn(function_list_settings, LV_SYMBOL_RIGHT, "Pengaturan Wifi");
    lv_obj_add_event_cb(btn_setting_wifi, cb_landing_on_config_wifi, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_setting_time = lv_list_add_btn(function_list_settings, LV_SYMBOL_RIGHT, "Pengaturan Waktu");
    lv_obj_add_event_cb(btn_setting_time, cb_landing_on_config_time, LV_EVENT_CLICKED, NULL);

    // setup menu_about
    lv_obj_set_x(content_menu_about, 0);
    lv_obj_set_y(content_menu_about, 0);

    lv_obj_t *container_about = lv_obj_create(content_menu_about);
    lv_obj_set_size(container_about, lv_pct(100), lv_pct(100));
    lv_obj_set_scrollbar_mode(container_about, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_flex_flow(container_about, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container_about, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(container_about, 2, LV_STATE_DEFAULT);

    lv_obj_t *about_title = lv_label_create(container_about);
    lv_obj_set_style_text_font(about_title, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_label_set_text(about_title, "Mirocle");

    String desc;
    desc = "MIROCLE (Mirror Therapy and Stationary Cycle) ";
    desc += "yang dikembangkan dalam penelitian ini berbasis IoT yang mampu ";
    desc += "memonitoring perkembangan pasien pasca stroke, sehingga tenaga medis ";
    desc += "dengan mudah memonitoring pasien pasca stroke.Parameter yang akan di ";
    desc += "monitoring dalam MIROCLE, yaitu detak jantung, saturasi oksigen, jumlah ";
    desc += "kalori yang terbakar, jumlah putaran pedal, dan durasi saat melakukan terapi";
    lv_obj_t *about_description = lv_label_create(container_about);
    lv_obj_set_style_text_font(about_description, &lv_font_montserrat_10, LV_STATE_DEFAULT);
    lv_obj_set_width(about_description, lv_pct(100));
    lv_label_set_text(about_description, desc.c_str());
    lv_label_set_long_mode(about_description, LV_LABEL_LONG_WRAP);

    view_state = VIEW_STATE_LANDING;
}

#endif