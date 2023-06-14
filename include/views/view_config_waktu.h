#ifndef _included_view_config_waktu
#define _included_view_config_waktu
#include "display_config.h"
#include "machine_manager/TimeManager.h"

void view_launch_config_waktu()
{
    screen_content_clear();
    _screen_content = lv_win_create(lv_scr_act(), 40);
    lv_obj_set_height(_screen_content, LV_VER_RES - STATUS_BAR_HEIGHT);
    lv_obj_set_style_pad_all(lv_win_get_header(_screen_content), 6, LV_STATE_DEFAULT);

    lv_win_add_title(_screen_content, "Pengaturan Waktu");
    lv_obj_t *btn_keluar = lv_win_add_btn(_screen_content, LV_SYMBOL_CLOSE, 40);
    lv_obj_add_event_cb(btn_keluar, cb_config_waktu_on_close, LV_EVENT_CLICKED, NULL);

    // current time setting
    lv_obj_t *container = lv_win_get_content(_screen_content);
    lv_obj_set_size(container, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_column(container, 4, LV_STATE_DEFAULT);

    // = time detail
    lv_obj_t *time_detail = lv_obj_create(container);
    lv_obj_set_size(time_detail, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(time_detail, LV_FLEX_FLOW_COLUMN);

    // == time detail time
    time_detail_time = lv_label_create(time_detail);
    lv_label_set_text(time_detail_time, "");

    // == time detail location
    lv_obj_t *time_detail_dropdown = lv_dropdown_create(time_detail);
    lv_dropdown_clear_options(time_detail_dropdown);
    lv_obj_set_width(time_detail_dropdown, LV_PCT(100));

    for (uint8_t i = 0; i < TIME_ZONE_COUNT; i++)
    {
        lv_dropdown_add_option(time_detail_dropdown, time_zone_list[i].name, i);
    }
    lv_dropdown_set_selected(time_detail_dropdown, time_get_zone_id());

    // = time sync btn
    lv_obj_t *time_btn_sync = lv_btn_create(container);
    lv_obj_t *time_detail_location_label = lv_label_create(time_btn_sync);
    lv_label_set_text(time_detail_location_label, "Sinkronkan Waktu");
    lv_obj_set_style_bg_color(time_btn_sync, lv_color_hex(0x444), LV_STATE_DISABLED);

    lv_obj_add_event_cb(time_btn_sync, cb_config_waktu_on_sync, LV_EVENT_CLICKED, time_detail_dropdown);

    view_state = VIEW_STATE_CONFIG_WAKTU;
}

#endif