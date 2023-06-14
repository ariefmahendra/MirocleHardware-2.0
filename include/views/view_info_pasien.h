#ifndef _included_view_info_pasien
#define _included_view_info_pasien
#include <Arduino.h>
#include <lvgl.h>
#include "display_config.h"
#include "machine_manager/PasienManager.h"
/**
 * SECTION INFO PASIEN
 */
void view_launch_info_pasien()
{
    screen_content_clear();
    _screen_content = lv_win_create(lv_scr_act(), 40);
    lv_obj_set_height(_screen_content, LV_VER_RES - STATUS_BAR_HEIGHT);
    lv_obj_set_style_pad_all(lv_win_get_header(_screen_content), 6, LV_STATE_DEFAULT);

    lv_win_add_title(_screen_content, "Informasi Pasien");

    lv_obj_t *btn_close = lv_win_add_btn(_screen_content, LV_SYMBOL_CLOSE, 40);
    lv_obj_add_event_cb(btn_close, cb_info_pasien_on_close, LV_EVENT_CLICKED, NULL);

    // content
    lv_obj_t *info_pasien_container = lv_obj_create(lv_win_get_content(_screen_content));
    lv_obj_set_size(info_pasien_container, lv_pct(100), LV_SIZE_CONTENT);

    lv_obj_t *label_nama_pasien = lv_label_create(info_pasien_container);
    lv_label_set_text_fmt(label_nama_pasien, "%s", pasien.nama);
    lv_obj_t *usia = lv_label_create(info_pasien_container);
    lv_label_set_text_fmt(usia, "%d tahun", pasien.usia);
    lv_obj_t *jenis_kelamin = lv_label_create(info_pasien_container);
    lv_label_set_text_fmt(jenis_kelamin, "%s", (pasien.jenis_kelamin == LAKI_LAKI ? "Laki-laki" : "Perempuan"));
    lv_obj_t *label_berat_badan = lv_label_create(info_pasien_container);
    lv_label_set_text_fmt(label_berat_badan, "%1.0f kg", pasien.berat_badan);

    lv_obj_set_flex_flow(info_pasien_container, LV_FLEX_FLOW_COLUMN);

    // lv_obj_t *field_berat_badan = lv_obj_create(lv_win_get_content(_screen_content));
    // lv_obj_set_size(field_berat_badan, lv_pct(100), LV_SIZE_CONTENT);

    // lv_obj_t *label_berat_badan = lv_label_create(field_berat_badan);
    // lv_label_set_text(label_berat_badan, "Berat badan");

    // lv_obj_t *keyboard;
    // keyboard = lv_keyboard_create(lv_scr_act());
    // lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_NUMBER);
    // lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

    // lv_obj_t *text_area = lv_textarea_create(field_berat_badan);
    // lv_textarea_set_one_line(text_area, true);
    // lv_obj_set_width(text_area, 50);
    // lv_textarea_set_max_length(text_area, 5);
    // lv_obj_add_event_cb(text_area, text_area_callback, LV_EVENT_ALL, keyboard);

    // lv_obj_t *label_satuan_berat_badan = lv_label_create(field_berat_badan);
    // lv_label_set_text(label_satuan_berat_badan, "kg");
    // lv_obj_set_flex_flow(field_berat_badan, LV_FLEX_FLOW_ROW);
    // lv_obj_set_flex_align(field_berat_badan, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *btn_ok_info = lv_btn_create(lv_win_get_content(_screen_content));
    lv_obj_t *btn_ok_label = lv_label_create(btn_ok_info);
    lv_label_set_text(btn_ok_label, LV_SYMBOL_OK);
    lv_obj_add_event_cb(btn_ok_info, cb_info_pasien_data_ok, LV_EVENT_CLICKED, NULL);
    lv_obj_align(btn_ok_info, LV_ALIGN_BOTTOM_RIGHT, -LV_DPX(15), -LV_DPX(15));
    lv_obj_add_flag(btn_ok_info, LV_OBJ_FLAG_FLOATING | LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_flex_flow(lv_win_get_content(_screen_content), LV_FLEX_FLOW_COLUMN);
    view_state = VIEW_STATE_TERAPI_INFO_PASIEN;
}

#endif