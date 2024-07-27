#include "esp_log.h"
#include <WZ8048C050.h>
#include "lvgl.h"
#include "ui/ui.h"
#include "ui/ui_events.h"

#define TAG "AstrOs Screen"

//=========== Tasks ===========
void lvglTask(void *arg);

//=========== UI Methods ===========

void onScript1(lv_event_t *e) {}
void onScript2(lv_event_t *e) {}
void onScript3(lv_event_t *e) {}
void onScript4(lv_event_t *e) {}
void onScript5(lv_event_t *e) {}
void onScript6(lv_event_t *e) {}
void onScript7(lv_event_t *e) {}
void onScript8(lv_event_t *e) {}
void onScript9(lv_event_t *e) {}
void onPanicStop(lv_event_t *e) {}
void onBack(lv_event_t *e) {}
void onForward(lv_event_t *e) {}

void onWifiScan(lv_event_t *e)
{

  lv_obj_clear_flag(ui_settingsscreen_spnwifi, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ui_settingsscreen_btnwifiscan, LV_OBJ_FLAG_HIDDEN);

 // wifiScanCallback();

/*
  astros_wifi_message_t msg;
  msg.type = AstrOsWifiMessageType::SCAN_START;
  msg.message = (char *)malloc(1);
  if (xQueueSend(wifiQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
  {
    free(msg.message);
  }
  */
}

void onWifiConnect(lv_event_t *e) {}
void onSyncScripts(lv_event_t *e) {}

void onKbdPressed(lv_event_t *e)
{
  auto *kbd = lv_event_get_target(e);
  auto btn_id = lv_keyboard_get_selected_btn(kbd);
  auto txt = lv_keyboard_get_btn_text(kbd, btn_id);

  // 39 - check
  // 22 - enter
  // 35 - kbd
  if (btn_id == 22)
  {
    auto isUserFocused = lv_obj_get_state(ui_settingsscreen_txtuser);
    if (isUserFocused)
    {
      lv_obj_clear_state(ui_settingsscreen_txtuser, LV_STATE_FOCUSED);
      lv_obj_add_state(ui_settingsscreen_txtpassword, LV_STATE_FOCUSED);
      lv_keyboard_set_textarea(kbd, ui_settingsscreen_txtpassword);
    }
    else
    {
      lv_obj_clear_state(ui_settingsscreen_txtpassword, LV_STATE_FOCUSED);
      lv_obj_add_state(ui_settingsscreen_txtuser, LV_STATE_FOCUSED);
      lv_keyboard_set_textarea(kbd, ui_settingsscreen_txtuser);
    }
  }
  else if (btn_id == 39 || btn_id == 35)
  {
    lv_obj_clear_state(ui_settingsscreen_txtuser, LV_STATE_FOCUSED);
    lv_obj_clear_state(ui_settingsscreen_txtpassword, LV_STATE_FOCUSED);
    lv_obj_add_flag(kbd, LV_OBJ_FLAG_HIDDEN);
  }
}

extern "C"
{
    void app_main(void);
}

void app_main()
{
  WZ8048C050_Init();
  ui_init(disp);

  xTaskCreate(&lvglTask, "lvglTask", 4096, NULL, 8, NULL);
}


void lvglTask(void *arg)
{
  while (1) {
    // raise the task priority of LVGL and/or reduce the handler period can
    // improve the performance
    vTaskDelay(pdMS_TO_TICKS(10));
    // The task running lv_timer_handler should have lower priority than that
    // running `lv_tick_inc`
    lv_timer_handler();
  }
}
