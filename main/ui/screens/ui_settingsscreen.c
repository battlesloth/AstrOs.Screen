// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.1
// LVGL version: 8.3.11
// Project name: AstrOsScreen

#include "../ui.h"

void ui_settingsscreen_screen_init(void)
{
    ui_settingsscreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_settingsscreen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_settingsscreen_cbxssids = lv_dropdown_create(ui_settingsscreen);
    lv_dropdown_set_options(ui_settingsscreen_cbxssids, "");
    lv_dropdown_set_text(ui_settingsscreen_cbxssids, "SSID");
    lv_obj_set_width(ui_settingsscreen_cbxssids, 485);
    lv_obj_set_height(ui_settingsscreen_cbxssids, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_settingsscreen_cbxssids, 60);
    lv_obj_set_y(ui_settingsscreen_cbxssids, 24);
    lv_obj_add_flag(ui_settingsscreen_cbxssids, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_set_style_text_font(ui_settingsscreen_cbxssids, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_font(lv_dropdown_get_list(ui_settingsscreen_cbxssids), &lv_font_montserrat_28,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingsscreen_txtuser = lv_textarea_create(ui_settingsscreen);
    lv_obj_set_width(ui_settingsscreen_txtuser, 480);
    lv_obj_set_height(ui_settingsscreen_txtuser, LV_SIZE_CONTENT);    /// 70
    lv_obj_set_x(ui_settingsscreen_txtuser, 60);
    lv_obj_set_y(ui_settingsscreen_txtuser, 104);
    lv_textarea_set_placeholder_text(ui_settingsscreen_txtuser, "Username");
    lv_textarea_set_one_line(ui_settingsscreen_txtuser, true);
    lv_obj_set_style_text_font(ui_settingsscreen_txtuser, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);



    ui_settingsscreen_txtpassword = lv_textarea_create(ui_settingsscreen);
    lv_obj_set_width(ui_settingsscreen_txtpassword, 480);
    lv_obj_set_height(ui_settingsscreen_txtpassword, LV_SIZE_CONTENT);    /// 70
    lv_obj_set_x(ui_settingsscreen_txtpassword, 60);
    lv_obj_set_y(ui_settingsscreen_txtpassword, 184);
    lv_textarea_set_placeholder_text(ui_settingsscreen_txtpassword, "Password");
    lv_textarea_set_one_line(ui_settingsscreen_txtpassword, true);
    lv_textarea_set_password_mode(ui_settingsscreen_txtpassword, true);
    lv_obj_set_style_text_font(ui_settingsscreen_txtpassword, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);



    ui_settingsscreen_btnwifiscan = lv_btn_create(ui_settingsscreen);
    lv_obj_set_width(ui_settingsscreen_btnwifiscan, 180);
    lv_obj_set_height(ui_settingsscreen_btnwifiscan, 70);
    lv_obj_set_x(ui_settingsscreen_btnwifiscan, -40);
    lv_obj_set_y(ui_settingsscreen_btnwifiscan, 16);
    lv_obj_set_align(ui_settingsscreen_btnwifiscan, LV_ALIGN_TOP_RIGHT);

    ui_settingsscreen_lblwifiscan = lv_label_create(ui_settingsscreen_btnwifiscan);
    lv_obj_set_width(ui_settingsscreen_lblwifiscan, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_settingsscreen_lblwifiscan, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_settingsscreen_lblwifiscan, LV_ALIGN_CENTER);
    lv_label_set_text(ui_settingsscreen_lblwifiscan, "WiFi Scan");
    lv_obj_set_style_text_font(ui_settingsscreen_lblwifiscan, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingsscreen_spnwifi = lv_spinner_create(ui_settingsscreen, 1000, 90);
    lv_obj_set_width(ui_settingsscreen_spnwifi, 70);
    lv_obj_set_height(ui_settingsscreen_spnwifi, 70);
    lv_obj_set_x(ui_settingsscreen_spnwifi, -90);
    lv_obj_set_y(ui_settingsscreen_spnwifi, 15);
    lv_obj_set_align(ui_settingsscreen_spnwifi, LV_ALIGN_TOP_RIGHT);
    lv_obj_add_flag(ui_settingsscreen_spnwifi, LV_OBJ_FLAG_HIDDEN);     /// Flags
    lv_obj_clear_flag(ui_settingsscreen_spnwifi, LV_OBJ_FLAG_CLICKABLE);      /// Flags

    lv_obj_set_style_arc_color(ui_settingsscreen_spnwifi, lv_color_hex(0x55B44D), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_settingsscreen_spnwifi, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    ui_settingsscreen_btnwificonnect = lv_btn_create(ui_settingsscreen);
    lv_obj_set_width(ui_settingsscreen_btnwificonnect, 180);
    lv_obj_set_height(ui_settingsscreen_btnwificonnect, 70);
    lv_obj_set_x(ui_settingsscreen_btnwificonnect, -40);
    lv_obj_set_y(ui_settingsscreen_btnwificonnect, 96);
    lv_obj_set_align(ui_settingsscreen_btnwificonnect, LV_ALIGN_TOP_RIGHT);

    ui_settingsscreen_lblwificonnect = lv_label_create(ui_settingsscreen_btnwificonnect);
    lv_obj_set_width(ui_settingsscreen_lblwificonnect, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_settingsscreen_lblwificonnect, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_settingsscreen_lblwificonnect, LV_ALIGN_CENTER);
    lv_label_set_text(ui_settingsscreen_lblwificonnect, "Connect");
    lv_obj_set_style_text_font(ui_settingsscreen_lblwificonnect, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingsscreen_btnsync = lv_btn_create(ui_settingsscreen);
    lv_obj_set_width(ui_settingsscreen_btnsync, 180);
    lv_obj_set_height(ui_settingsscreen_btnsync, 70);
    lv_obj_set_x(ui_settingsscreen_btnsync, -40);
    lv_obj_set_y(ui_settingsscreen_btnsync, 176);
    lv_obj_set_align(ui_settingsscreen_btnsync, LV_ALIGN_TOP_RIGHT);

    ui_settingsscreen_lblsync = lv_label_create(ui_settingsscreen_btnsync);
    lv_obj_set_width(ui_settingsscreen_lblsync, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_settingsscreen_lblsync, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_settingsscreen_lblsync, LV_ALIGN_CENTER);
    lv_label_set_text(ui_settingsscreen_lblsync, "Sync");
    lv_obj_set_style_text_font(ui_settingsscreen_lblsync, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingsscreen_btnclose = lv_btn_create(ui_settingsscreen);
    lv_obj_set_width(ui_settingsscreen_btnclose, 180);
    lv_obj_set_height(ui_settingsscreen_btnclose, 70);
    lv_obj_set_x(ui_settingsscreen_btnclose, -40);
    lv_obj_set_y(ui_settingsscreen_btnclose, -20);
    lv_obj_set_align(ui_settingsscreen_btnclose, LV_ALIGN_BOTTOM_RIGHT);

    ui_settingsscreen_lblclose = lv_label_create(ui_settingsscreen_btnclose);
    lv_obj_set_width(ui_settingsscreen_lblclose, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_settingsscreen_lblclose, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_settingsscreen_lblclose, LV_ALIGN_CENTER);
    lv_label_set_text(ui_settingsscreen_lblclose, "Close");
    lv_obj_set_style_text_font(ui_settingsscreen_lblclose, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingsscreen_kbdsettings = lv_keyboard_create(ui_settingsscreen);
    lv_obj_set_width(ui_settingsscreen_kbdsettings, 800);
    lv_obj_set_height(ui_settingsscreen_kbdsettings, 240);
    lv_obj_set_align(ui_settingsscreen_kbdsettings, LV_ALIGN_BOTTOM_MID);
    lv_obj_add_flag(ui_settingsscreen_kbdsettings, LV_OBJ_FLAG_HIDDEN);     /// Flags

    lv_obj_add_event_cb(ui_settingsscreen_txtuser, ui_event_settingsscreen_txtuser, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsscreen_txtpassword, ui_event_settingsscreen_txtpassword, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsscreen_btnwifiscan, ui_event_settingsscreen_btnwifiscan, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsscreen_btnwificonnect, ui_event_settingsscreen_btnwificonnect, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsscreen_btnsync, ui_event_settingsscreen_btnsync, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsscreen_btnclose, ui_event_settingsscreen_btnclose, LV_EVENT_ALL, NULL);
    lv_keyboard_set_textarea(ui_settingsscreen_kbdsettings, ui_settingsscreen_txtuser);
    lv_obj_add_event_cb(ui_settingsscreen_kbdsettings, ui_event_settingsscreen_kbdsettings, LV_EVENT_ALL, NULL);

}