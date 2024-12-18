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
    lv_obj_set_width(ui_settingsscreen_cbxssids, 380);
    lv_obj_set_height(ui_settingsscreen_cbxssids, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_settingsscreen_cbxssids, 12);
    lv_obj_set_y(ui_settingsscreen_cbxssids, 12);
    lv_obj_add_flag(ui_settingsscreen_cbxssids, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_set_scroll_dir(ui_settingsscreen_cbxssids, LV_DIR_VER);
    lv_obj_set_style_text_font(ui_settingsscreen_cbxssids, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_font(lv_dropdown_get_list(ui_settingsscreen_cbxssids), &lv_font_montserrat_28,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingsscreen_txtpassword = lv_textarea_create(ui_settingsscreen);
    lv_obj_set_width(ui_settingsscreen_txtpassword, 380);
    lv_obj_set_height(ui_settingsscreen_txtpassword, LV_SIZE_CONTENT);    /// 70
    lv_obj_set_x(ui_settingsscreen_txtpassword, 12);
    lv_obj_set_y(ui_settingsscreen_txtpassword, 90);
    lv_textarea_set_placeholder_text(ui_settingsscreen_txtpassword, "Password");
    lv_textarea_set_one_line(ui_settingsscreen_txtpassword, true);
    lv_textarea_set_password_mode(ui_settingsscreen_txtpassword, true);
    lv_obj_set_style_text_font(ui_settingsscreen_txtpassword, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);



    ui_settingsscreen_btnwifiscan = lv_btn_create(ui_settingsscreen);
    lv_obj_set_width(ui_settingsscreen_btnwifiscan, 180);
    lv_obj_set_height(ui_settingsscreen_btnwifiscan, 70);
    lv_obj_set_x(ui_settingsscreen_btnwifiscan, -292);
    lv_obj_set_y(ui_settingsscreen_btnwifiscan, -18);
    lv_obj_set_align(ui_settingsscreen_btnwifiscan, LV_ALIGN_BOTTOM_MID);

    ui_settingsscreen_lblwifiscan = lv_label_create(ui_settingsscreen_btnwifiscan);
    lv_obj_set_width(ui_settingsscreen_lblwifiscan, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_settingsscreen_lblwifiscan, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_settingsscreen_lblwifiscan, LV_ALIGN_CENTER);
    lv_label_set_text(ui_settingsscreen_lblwifiscan, "WiFi Scan");
    lv_obj_set_style_text_font(ui_settingsscreen_lblwifiscan, &ui_font_StarJedi_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingsscreen_spnwifi = lv_spinner_create(ui_settingsscreen, 1000, 90);
    lv_obj_set_width(ui_settingsscreen_spnwifi, 70);
    lv_obj_set_height(ui_settingsscreen_spnwifi, 70);
    lv_obj_set_x(ui_settingsscreen_spnwifi, -658);
    lv_obj_set_y(ui_settingsscreen_spnwifi, 392);
    lv_obj_set_align(ui_settingsscreen_spnwifi, LV_ALIGN_TOP_RIGHT);
    lv_obj_add_flag(ui_settingsscreen_spnwifi, LV_OBJ_FLAG_HIDDEN);     /// Flags
    lv_obj_clear_flag(ui_settingsscreen_spnwifi, LV_OBJ_FLAG_CLICKABLE);      /// Flags

    lv_obj_set_style_arc_color(ui_settingsscreen_spnwifi, lv_color_hex(0x55B44D), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_settingsscreen_spnwifi, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    ui_settingsscreen_btnwificonnect = lv_btn_create(ui_settingsscreen);
    lv_obj_set_width(ui_settingsscreen_btnwificonnect, 180);
    lv_obj_set_height(ui_settingsscreen_btnwificonnect, 70);
    lv_obj_set_x(ui_settingsscreen_btnwificonnect, -98);
    lv_obj_set_y(ui_settingsscreen_btnwificonnect, -18);
    lv_obj_set_align(ui_settingsscreen_btnwificonnect, LV_ALIGN_BOTTOM_MID);

    ui_settingsscreen_lblwificonnect = lv_label_create(ui_settingsscreen_btnwificonnect);
    lv_obj_set_width(ui_settingsscreen_lblwificonnect, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_settingsscreen_lblwificonnect, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_settingsscreen_lblwificonnect, LV_ALIGN_CENTER);
    lv_label_set_text(ui_settingsscreen_lblwificonnect, "Connect");
    lv_obj_set_style_text_font(ui_settingsscreen_lblwificonnect, &ui_font_StarJedi_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingsscreen_btnsync = lv_btn_create(ui_settingsscreen);
    lv_obj_set_width(ui_settingsscreen_btnsync, 180);
    lv_obj_set_height(ui_settingsscreen_btnsync, 70);
    lv_obj_set_x(ui_settingsscreen_btnsync, 98);
    lv_obj_set_y(ui_settingsscreen_btnsync, -18);
    lv_obj_set_align(ui_settingsscreen_btnsync, LV_ALIGN_BOTTOM_MID);

    ui_settingsscreen_lblsync = lv_label_create(ui_settingsscreen_btnsync);
    lv_obj_set_width(ui_settingsscreen_lblsync, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_settingsscreen_lblsync, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_settingsscreen_lblsync, LV_ALIGN_CENTER);
    lv_label_set_text(ui_settingsscreen_lblsync, "Sync");
    lv_obj_set_style_text_font(ui_settingsscreen_lblsync, &ui_font_StarJedi_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingsscreen_btnclose = lv_btn_create(ui_settingsscreen);
    lv_obj_set_width(ui_settingsscreen_btnclose, 180);
    lv_obj_set_height(ui_settingsscreen_btnclose, 70);
    lv_obj_set_x(ui_settingsscreen_btnclose, 292);
    lv_obj_set_y(ui_settingsscreen_btnclose, -18);
    lv_obj_set_align(ui_settingsscreen_btnclose, LV_ALIGN_BOTTOM_MID);

    ui_settingsscreen_lblclose = lv_label_create(ui_settingsscreen_btnclose);
    lv_obj_set_width(ui_settingsscreen_lblclose, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_settingsscreen_lblclose, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_settingsscreen_lblclose, LV_ALIGN_CENTER);
    lv_label_set_text(ui_settingsscreen_lblclose, "Close");
    lv_obj_set_style_text_font(ui_settingsscreen_lblclose, &ui_font_StarJedi_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingsscreen_txtapikey = lv_textarea_create(ui_settingsscreen);
    lv_obj_set_width(ui_settingsscreen_txtapikey, 380);
    lv_obj_set_height(ui_settingsscreen_txtapikey, LV_SIZE_CONTENT);    /// 70
    lv_obj_set_x(ui_settingsscreen_txtapikey, -12);
    lv_obj_set_y(ui_settingsscreen_txtapikey, 12);
    lv_obj_set_align(ui_settingsscreen_txtapikey, LV_ALIGN_TOP_RIGHT);
    lv_textarea_set_placeholder_text(ui_settingsscreen_txtapikey, "API Key");
    lv_textarea_set_one_line(ui_settingsscreen_txtapikey, true);
    lv_obj_set_style_text_font(ui_settingsscreen_txtapikey, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);



    ui_settingsscreen_txtserverip = lv_textarea_create(ui_settingsscreen);
    lv_obj_set_width(ui_settingsscreen_txtserverip, 380);
    lv_obj_set_height(ui_settingsscreen_txtserverip, LV_SIZE_CONTENT);    /// 70
    lv_obj_set_x(ui_settingsscreen_txtserverip, -12);
    lv_obj_set_y(ui_settingsscreen_txtserverip, 90);
    lv_obj_set_align(ui_settingsscreen_txtserverip, LV_ALIGN_TOP_RIGHT);
    lv_textarea_set_placeholder_text(ui_settingsscreen_txtserverip, "Server IP");
    lv_textarea_set_one_line(ui_settingsscreen_txtserverip, true);
    lv_obj_add_state(ui_settingsscreen_txtserverip, LV_STATE_DISABLED);       /// States
    lv_obj_set_style_text_font(ui_settingsscreen_txtserverip, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);



    ui_settingsscreen_chkusegateway = lv_checkbox_create(ui_settingsscreen);
    lv_checkbox_set_text(ui_settingsscreen_chkusegateway, "Using WiFi Router");
    lv_obj_set_width(ui_settingsscreen_chkusegateway, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_settingsscreen_chkusegateway, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_settingsscreen_chkusegateway, -47);
    lv_obj_set_y(ui_settingsscreen_chkusegateway, 166);
    lv_obj_set_align(ui_settingsscreen_chkusegateway, LV_ALIGN_TOP_RIGHT);
    lv_obj_add_flag(ui_settingsscreen_chkusegateway, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_set_style_text_font(ui_settingsscreen_chkusegateway, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingsscreen_kbdsettings = lv_keyboard_create(ui_settingsscreen);
    lv_obj_set_width(ui_settingsscreen_kbdsettings, 800);
    lv_obj_set_height(ui_settingsscreen_kbdsettings, 240);
    lv_obj_set_align(ui_settingsscreen_kbdsettings, LV_ALIGN_BOTTOM_MID);
    lv_obj_add_state(ui_settingsscreen_kbdsettings, LV_STATE_USER_3);       /// States
    lv_obj_add_flag(ui_settingsscreen_kbdsettings, LV_OBJ_FLAG_HIDDEN);     /// Flags

    ui_settingsscreen_settingserrormodal = lv_obj_create(ui_settingsscreen);
    lv_obj_remove_style_all(ui_settingsscreen_settingserrormodal);
    lv_obj_set_width(ui_settingsscreen_settingserrormodal, 800);
    lv_obj_set_height(ui_settingsscreen_settingserrormodal, 480);
    lv_obj_set_align(ui_settingsscreen_settingserrormodal, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_settingsscreen_settingserrormodal, LV_OBJ_FLAG_HIDDEN);     /// Flags
    lv_obj_clear_flag(ui_settingsscreen_settingserrormodal, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_settingsscreen_coverpanel2 = lv_obj_create(ui_settingsscreen_settingserrormodal);
    lv_obj_set_width(ui_settingsscreen_coverpanel2, 800);
    lv_obj_set_height(ui_settingsscreen_coverpanel2, 480);
    lv_obj_set_align(ui_settingsscreen_coverpanel2, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_settingsscreen_coverpanel2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_settingsscreen_coverpanel2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_settingsscreen_coverpanel2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_settingsscreen_coverpanel2, 200, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingsscreen_btnerrorack = lv_btn_create(ui_settingsscreen_coverpanel2);
    lv_obj_set_width(ui_settingsscreen_btnerrorack, 400);
    lv_obj_set_height(ui_settingsscreen_btnerrorack, 200);
    lv_obj_set_align(ui_settingsscreen_btnerrorack, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_settingsscreen_btnerrorack, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_settingsscreen_btnerrorack, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_settingsscreen_btnerrorack, lv_color_hex(0x353535), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_settingsscreen_btnerrorack, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingsscreen_lblerrormessage = lv_label_create(ui_settingsscreen_btnerrorack);
    lv_obj_set_width(ui_settingsscreen_lblerrormessage, 340);
    lv_obj_set_height(ui_settingsscreen_lblerrormessage, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_settingsscreen_lblerrormessage, LV_ALIGN_CENTER);
    lv_obj_set_style_text_align(ui_settingsscreen_lblerrormessage, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_settingsscreen_lblerrormessage, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_settingsscreen_cbxssids, ui_event_settingsscreen_cbxssids, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsscreen_txtpassword, ui_event_settingsscreen_txtpassword, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsscreen_btnwifiscan, ui_event_settingsscreen_btnwifiscan, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsscreen_btnwificonnect, ui_event_settingsscreen_btnwificonnect, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsscreen_btnsync, ui_event_settingsscreen_btnsync, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsscreen_btnclose, ui_event_settingsscreen_btnclose, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsscreen_txtapikey, ui_event_settingsscreen_txtapikey, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsscreen_txtserverip, ui_event_settingsscreen_txtserverip, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsscreen_chkusegateway, ui_event_settingsscreen_chkusegateway, LV_EVENT_ALL, NULL);
    lv_keyboard_set_textarea(ui_settingsscreen_kbdsettings, ui_settingsscreen_txtpassword);
    lv_obj_add_event_cb(ui_settingsscreen_kbdsettings, ui_event_settingsscreen_kbdsettings, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsscreen_lblerrormessage, ui_event_settingsscreen_lblerrormessage, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settingsscreen_btnerrorack, ui_event_settingsscreen_btnerrorack, LV_EVENT_ALL, NULL);

}
