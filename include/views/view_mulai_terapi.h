#ifndef _included_view_mulai_terapi
#define _included_view_mulai_terapi
#include <Arduino.h>
#include <lvgl.h>
#include "display_config.h"

/**
 * SECTION TERAPI
 */
void view_launch_terapi()
{
    static lv_coord_t kolom[] = {LV_GRID_FR(3), LV_GRID_FR(3), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t baris[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t tile_col[] = {LV_GRID_FR(1), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t tile_row[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    static lv_color_t tile_color = lv_color_hex(0x73DFFB);

    LV_IMG_DECLARE(img_calories);
    LV_IMG_DECLARE(img_clock);
    LV_IMG_DECLARE(img_heartbeat);
    LV_IMG_DECLARE(img_o2);
    LV_IMG_DECLARE(img_start_button);
    LV_IMG_DECLARE(img_stop);
    LV_IMG_DECLARE(img_sync);

    screen_content_clear();
    _screen_content = lv_obj_create(lv_scr_act());
    lv_obj_set_size(_screen_content, LV_HOR_RES, LV_VER_RES - STATUS_BAR_HEIGHT);
    lv_obj_center(_screen_content);
    lv_obj_set_grid_dsc_array(_screen_content, kolom, baris);
    lv_obj_set_style_pad_all(_screen_content, 6, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(_screen_content, 4, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(_screen_content, 4, LV_STATE_DEFAULT);

    // TILE INFO CALORY
    lv_obj_t *tile_info_calory = lv_obj_create(_screen_content);
    lv_obj_set_style_bg_color(tile_info_calory, tile_color, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tile_info_calory, 6, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(tile_info_calory, 4, LV_STATE_DEFAULT);
    lv_obj_set_grid_dsc_array(tile_info_calory, tile_col, tile_row);

    lv_obj_t *info_calory_icon = lv_img_create(tile_info_calory);
    lv_img_set_src(info_calory_icon, &img_calories);
    lv_img_set_zoom(info_calory_icon, 90);
    lv_img_set_size_mode(info_calory_icon, LV_IMG_SIZE_MODE_REAL);
    lv_obj_set_grid_cell(info_calory_icon, LV_GRID_ALIGN_CENTER, 0, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);
    info_calory_label = lv_label_create(tile_info_calory);
    lv_obj_set_style_text_font(info_calory_label, &lv_font_montserrat_12, LV_STATE_DEFAULT);
    lv_label_set_text_static(info_calory_label, view_var_calory.c_str());

    lv_obj_set_grid_cell(info_calory_label, LV_GRID_ALIGN_STRETCH, 1, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(tile_info_calory, LV_GRID_ALIGN_STRETCH, 0, 1,
                         LV_GRID_ALIGN_STRETCH, 0, 1);

    // TILE OXYGEN
    lv_obj_t *tile_info_oxygen = lv_obj_create(_screen_content);
    lv_obj_set_style_bg_color(tile_info_oxygen, tile_color, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tile_info_oxygen, 6, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(tile_info_oxygen, 4, LV_STATE_DEFAULT);
    lv_obj_set_grid_dsc_array(tile_info_oxygen, tile_col, tile_row);

    lv_obj_t *info_oxygen_icon = lv_img_create(tile_info_oxygen);
    lv_img_set_src(info_oxygen_icon, &img_o2);
    lv_img_set_zoom(info_oxygen_icon, 90);
    lv_img_set_size_mode(info_oxygen_icon, LV_IMG_SIZE_MODE_REAL);
    lv_obj_set_grid_cell(info_oxygen_icon, LV_GRID_ALIGN_CENTER, 0, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);
    info_oxygen_label = lv_label_create(tile_info_oxygen);
    lv_obj_set_style_text_font(info_oxygen_label, &lv_font_montserrat_12, LV_STATE_DEFAULT);
    lv_label_set_text_static(info_oxygen_label, view_var_o2.c_str());

    lv_obj_set_grid_cell(info_oxygen_label, LV_GRID_ALIGN_STRETCH, 1, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(tile_info_oxygen, LV_GRID_ALIGN_STRETCH, 0, 1,
                         LV_GRID_ALIGN_STRETCH, 1, 1);

    // TILE ROTATION
    lv_obj_t *tile_info_rotation = lv_obj_create(_screen_content);
    lv_obj_set_style_bg_color(tile_info_rotation, tile_color, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tile_info_rotation, 6, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(tile_info_rotation, 4, LV_STATE_DEFAULT);
    lv_obj_set_grid_dsc_array(tile_info_rotation, tile_col, tile_row);

    lv_obj_t *info_rotation_icon = lv_img_create(tile_info_rotation);
    lv_img_set_src(info_rotation_icon, &img_sync);
    lv_img_set_zoom(info_rotation_icon, 50);
    lv_img_set_size_mode(info_rotation_icon, LV_IMG_SIZE_MODE_REAL);
    lv_obj_set_grid_cell(info_rotation_icon, LV_GRID_ALIGN_CENTER, 0, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);
    info_rotation_label = lv_label_create(tile_info_rotation);
    lv_obj_set_style_text_font(info_rotation_label, &lv_font_montserrat_12, LV_STATE_DEFAULT);
    lv_label_set_text_static(info_rotation_label, view_var_rotation.c_str());

    lv_obj_set_grid_cell(info_rotation_label, LV_GRID_ALIGN_STRETCH, 1, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(tile_info_rotation, LV_GRID_ALIGN_STRETCH, 0, 1,
                         LV_GRID_ALIGN_STRETCH, 2, 1);

    // TILE WHAT
    lv_obj_t *tile_info_what = lv_obj_create(_screen_content);
    lv_obj_t *info_what_label = lv_label_create(tile_info_what);
    lv_label_set_text(info_what_label, "");
    lv_obj_set_grid_cell(tile_info_what, LV_GRID_ALIGN_STRETCH, 1, 1,
                         LV_GRID_ALIGN_STRETCH, 0, 1);

    // TILE HEART
    lv_obj_t *tile_info_heart = lv_obj_create(_screen_content);
    lv_obj_set_style_bg_color(tile_info_heart, tile_color, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tile_info_heart, 6, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(tile_info_heart, 4, LV_STATE_DEFAULT);
    lv_obj_set_grid_dsc_array(tile_info_heart, tile_col, tile_row);

    lv_obj_t *info_heart_icon = lv_img_create(tile_info_heart);
    lv_img_set_src(info_heart_icon, &img_heartbeat);
    lv_img_set_zoom(info_heart_icon, 100);
    lv_img_set_size_mode(info_heart_icon, LV_IMG_SIZE_MODE_REAL);
    lv_obj_set_grid_cell(info_heart_icon, LV_GRID_ALIGN_CENTER, 0, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);
    info_heart_label = lv_label_create(tile_info_heart);
    lv_obj_set_style_text_font(info_heart_label, &lv_font_montserrat_12, LV_STATE_DEFAULT);
    lv_label_set_text_static(info_heart_label, view_var_heartbeat.c_str());

    lv_obj_set_grid_cell(info_heart_label, LV_GRID_ALIGN_STRETCH, 1, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(tile_info_heart, LV_GRID_ALIGN_STRETCH, 1, 1,
                         LV_GRID_ALIGN_STRETCH, 1, 1);

    // TILE TIME
    lv_obj_t *tile_info_time = lv_obj_create(_screen_content);
    lv_obj_set_style_bg_color(tile_info_time, tile_color, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tile_info_time, 6, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(tile_info_time, 4, LV_STATE_DEFAULT);
    lv_obj_set_grid_dsc_array(tile_info_time, tile_col, tile_row);

    lv_obj_t *info_time_icon = lv_img_create(tile_info_time);
    lv_img_set_src(info_time_icon, &img_clock);
    lv_img_set_zoom(info_time_icon, 80);
    lv_img_set_size_mode(info_time_icon, LV_IMG_SIZE_MODE_REAL);
    lv_obj_set_grid_cell(info_time_icon, LV_GRID_ALIGN_CENTER, 0, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);
    info_time_label = lv_label_create(tile_info_time);
    lv_label_set_text_static(info_time_label, view_var_duration.c_str());

    lv_obj_set_grid_cell(info_time_label, LV_GRID_ALIGN_STRETCH, 1, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_style_text_font(info_time_label, &lv_font_montserrat_12, LV_STATE_DEFAULT);
    lv_obj_set_grid_cell(tile_info_time, LV_GRID_ALIGN_STRETCH, 1, 1,
                         LV_GRID_ALIGN_STRETCH, 2, 1);

    // TILE BUTTON
    tile_info_button = lv_btn_create(_screen_content);
    tile_info_btn_label = lv_label_create(tile_info_button);
    lv_obj_center(tile_info_btn_label);
    if (lv_obj_is_valid(tile_info_btn_label))
        lv_label_set_text_static(tile_info_btn_label, tile_btn_label_txt.c_str());
    lv_obj_add_event_cb(tile_info_button, cb_terapi_on_btn_press, LV_EVENT_CLICKED, NULL);

    lv_obj_set_grid_cell(tile_info_button, LV_GRID_ALIGN_STRETCH, 2, 1,
                         LV_GRID_ALIGN_STRETCH, 1, 2);

    // TILE STATUS
    tile_info_status = lv_obj_create(_screen_content);
    lv_obj_set_style_pad_all(tile_info_status, 6, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(tile_info_status, lv_color_hex(0x26D0FC), LV_STATE_DEFAULT);
    info_status_label = lv_label_create(tile_info_status);
    if (lv_obj_is_valid(info_status_label))
        lv_obj_center(info_status_label);
    lv_label_set_text_static(info_status_label, info_status_label_txt.c_str());

    lv_obj_set_grid_cell(tile_info_status, LV_GRID_ALIGN_STRETCH, 2, 1,
                         LV_GRID_ALIGN_STRETCH, 0, 1);
}

#endif