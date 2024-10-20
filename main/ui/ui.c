// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.1
// LVGL version: 8.3.11
// Project name: AstrOsScreen

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////


// SCREEN: ui_mainscreen
void ui_mainscreen_screen_init(void);
lv_obj_t * ui_mainscreen;
void ui_event_mainscreen_btnscript1(lv_event_t * e);
lv_obj_t * ui_mainscreen_btnscript1;
lv_obj_t * ui_mainscreen_lblscript1;
void ui_event_mainscreen_btnscript2(lv_event_t * e);
lv_obj_t * ui_mainscreen_btnscript2;
lv_obj_t * ui_mainscreen_lblscript2;
void ui_event_mainscreen_btnscript3(lv_event_t * e);
lv_obj_t * ui_mainscreen_btnscript3;
lv_obj_t * ui_mainscreen_lblscript3;
void ui_event_mainscreen_btnscript4(lv_event_t * e);
lv_obj_t * ui_mainscreen_btnscript4;
lv_obj_t * ui_mainscreen_lblscript4;
void ui_event_mainscreen_btnscript5(lv_event_t * e);
lv_obj_t * ui_mainscreen_btnscript5;
lv_obj_t * ui_mainscreen_lblscript5;
void ui_event_mainscreen_btnscript6(lv_event_t * e);
lv_obj_t * ui_mainscreen_btnscript6;
lv_obj_t * ui_mainscreen_lblscript6;
void ui_event_mainscreen_btnscript7(lv_event_t * e);
lv_obj_t * ui_mainscreen_btnscript7;
lv_obj_t * ui_mainscreen_lblscript7;
void ui_event_mainscreen_btnscript8(lv_event_t * e);
lv_obj_t * ui_mainscreen_btnscript8;
lv_obj_t * ui_mainscreen_lblscript8;
void ui_event_mainscreen_btnscript9(lv_event_t * e);
lv_obj_t * ui_mainscreen_btnscript9;
lv_obj_t * ui_mainscreen_lblscript9;
void ui_event_mainscreen_btnpanicstop(lv_event_t * e);
lv_obj_t * ui_mainscreen_btnpanicstop;
lv_obj_t * ui_mainscreen_lblpanicstop;
void ui_event_mainscreen_btnback(lv_event_t * e);
lv_obj_t * ui_mainscreen_btnback;
lv_obj_t * ui_mainscreen_image1;
void ui_event_mainscreen_btnforward(lv_event_t * e);
lv_obj_t * ui_mainscreen_btnforward;
lv_obj_t * ui_mainscreen_image2;
lv_obj_t * ui_mainscreen_btnwifi;
void ui_event_mainscreen_btnsettings(lv_event_t * e);
lv_obj_t * ui_mainscreen_btnsettings;
lv_obj_t * ui_mainscreen_image3;
lv_obj_t * ui_mainscreen_mainerrormodal;
lv_obj_t * ui_mainscreen_coverpanel1;
void ui_event_mainscreen_btnerrorack(lv_event_t * e);
lv_obj_t * ui_mainscreen_btnerrorack;
lv_obj_t * ui_mainscreen_lblerrormessage;


// SCREEN: ui_settingsscreen
void ui_settingsscreen_screen_init(void);
lv_obj_t * ui_settingsscreen;
void ui_event_settingsscreen_cbxssids(lv_event_t * e);
lv_obj_t * ui_settingsscreen_cbxssids;
void ui_event_settingsscreen_txtpassword(lv_event_t * e);
lv_obj_t * ui_settingsscreen_txtpassword;
void ui_event_settingsscreen_btnwifiscan(lv_event_t * e);
lv_obj_t * ui_settingsscreen_btnwifiscan;
lv_obj_t * ui_settingsscreen_lblwifiscan;
lv_obj_t * ui_settingsscreen_spnwifi;
void ui_event_settingsscreen_btnwificonnect(lv_event_t * e);
lv_obj_t * ui_settingsscreen_btnwificonnect;
lv_obj_t * ui_settingsscreen_lblwificonnect;
void ui_event_settingsscreen_btnsync(lv_event_t * e);
lv_obj_t * ui_settingsscreen_btnsync;
lv_obj_t * ui_settingsscreen_lblsync;
void ui_event_settingsscreen_btnclose(lv_event_t * e);
lv_obj_t * ui_settingsscreen_btnclose;
lv_obj_t * ui_settingsscreen_lblclose;
void ui_event_settingsscreen_kbdsettings(lv_event_t * e);
lv_obj_t * ui_settingsscreen_kbdsettings;
lv_obj_t * ui_settingsscreen_settingserrormodal;
lv_obj_t * ui_settingsscreen_coverpanel2;
void ui_event_settingsscreen_btnerrorack(lv_event_t * e);
lv_obj_t * ui_settingsscreen_btnerrorack;
void ui_event_settingsscreen_lblerrormessage(lv_event_t * e);
lv_obj_t * ui_settingsscreen_lblerrormessage;
void ui_event_settingsscreen_txtapikey(lv_event_t * e);
lv_obj_t * ui_settingsscreen_txtapikey;
lv_obj_t * ui____initial_actions0;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////
void ui_event_mainscreen_btnscript1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onScript1(e);
    }
}
void ui_event_mainscreen_btnscript2(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onScript2(e);
    }
}
void ui_event_mainscreen_btnscript3(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onScript3(e);
    }
}
void ui_event_mainscreen_btnscript4(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onScript4(e);
    }
}
void ui_event_mainscreen_btnscript5(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onScript5(e);
    }
}
void ui_event_mainscreen_btnscript6(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onScript6(e);
    }
}
void ui_event_mainscreen_btnscript7(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onScript7(e);
    }
}
void ui_event_mainscreen_btnscript8(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onScript8(e);
    }
}
void ui_event_mainscreen_btnscript9(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onScript9(e);
    }
}
void ui_event_mainscreen_btnpanicstop(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onPanicStop(e);
    }
}
void ui_event_mainscreen_btnback(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onBack(e);
    }
}
void ui_event_mainscreen_btnforward(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onForward(e);
    }
}
void ui_event_mainscreen_btnsettings(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_settingsscreen, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 500, 0, &ui_settingsscreen_screen_init);
    }
}
void ui_event_mainscreen_btnerrorack(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onErrorAck(e);
    }
}
void ui_event_settingsscreen_cbxssids(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED) {
        onCbxSSIDChanged(e);
    }
}
void ui_event_settingsscreen_txtpassword(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_FOCUSED) {
        _ui_keyboard_set_target(ui_settingsscreen_kbdsettings,  ui_settingsscreen_txtpassword);
    }
    if(event_code == LV_EVENT_CLICKED) {
        _ui_flag_modify(ui_settingsscreen_kbdsettings, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    }
}
void ui_event_settingsscreen_btnwifiscan(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onWifiScan(e);
    }
}
void ui_event_settingsscreen_btnwificonnect(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onWifiConnect(e);
    }
}
void ui_event_settingsscreen_btnsync(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onSyncScripts(e);
    }
}
void ui_event_settingsscreen_btnclose(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_mainscreen, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 500, 0, &ui_mainscreen_screen_init);
    }
}
void ui_event_settingsscreen_kbdsettings(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED) {
        onKbdPressed(e);
    }
}
void ui_event_settingsscreen_btnerrorack(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onErrorAck(e);
    }
}
void ui_event_settingsscreen_lblerrormessage(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        onErrorAck(e);
    }
}
void ui_event_settingsscreen_txtapikey(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_FOCUSED) {
        _ui_keyboard_set_target(ui_settingsscreen_kbdsettings,  ui_settingsscreen_txtapikey);
    }
    if(event_code == LV_EVENT_CLICKED) {
        _ui_flag_modify(ui_settingsscreen_kbdsettings, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    }
    if(event_code == LV_EVENT_DEFOCUSED) {
        onApiKeyLostFocus(e);
    }
}

///////////////////// SCREENS ////////////////////

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_mainscreen_screen_init();
    ui_settingsscreen_screen_init();
    ui____initial_actions0 = lv_obj_create(NULL);
    lv_disp_load_scr(ui_mainscreen);
}
