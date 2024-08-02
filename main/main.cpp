#include "esp_log.h"
#include <WZ8048C050.h>
#include "lvgl.h"
#include "ui/ui.h"
#include "ui/ui_events.h"

#include "messaging/astros_messaging.hpp"
#include "wifi_controller/wifi_controller.hpp"
#include "storage/storage_manager.hpp"
#include <nvs_flash.h>

#define TAG "AstrOs Screen"
#define QUEUE_LENGTH 10

SemaphoreHandle_t xGuiSemaphore;

//========== Queues ===========
static QueueHandle_t uiQueue;
static QueueHandle_t wifiQueue;

//========== Tasks ===========
void lvglTask(void *arg);
void uiUpdateTask(void *arg);
void wifiTask(void *arg);

extern "C"
{
  void app_main(void);
}

void app_main()
{

  xGuiSemaphore = xSemaphoreCreateMutex();

  ESP_ERROR_CHECK(storageManager.Init());

  WZ8048C050_Init();
  ui_init();

  wifiQueue = xQueueCreate(QUEUE_LENGTH, sizeof(astros_wifi_message_t));
  uiQueue = xQueueCreate(QUEUE_LENGTH, sizeof(astros_ui_message_t));

  wifiController.Init(uiQueue);

  xTaskCreate(&wifiTask, "wifiTask", 4096, (void *)wifiQueue, 9, NULL);

  xTaskCreate(&uiUpdateTask, "uiUpdateTask", 4096, (void *)uiQueue, 8, NULL);
  xTaskCreate(&lvglTask, "lvglTask", 4096, NULL, 10, NULL);
}

void lvglTask(void *arg)
{
  while (1)
  {
    vTaskDelay(pdMS_TO_TICKS(10));

    if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
    {
      lv_timer_handler();
      xSemaphoreGive(xGuiSemaphore);
    }
  }
}

void wifiTask(void *arg)
{
  auto queue = (QueueHandle_t)arg;
  astros_wifi_message_t msg;

  while (1)
  {
    if (xQueueReceive(queue, &msg, pdMS_TO_TICKS(500)) == pdTRUE)
    {
      auto highWaterMark = uxTaskGetStackHighWaterMark(NULL);
      if (highWaterMark < 500)
      {
        ESP_LOGW(TAG, "Wifi Task Stack HWM: %d", highWaterMark);
      }

      ESP_LOGI(TAG, "Received message: %d", (int)msg.type);
      switch (msg.type)
      {
      case AstrOsWifiMessageType::SCAN_START:
        wifiController.Scan();
        break;
      case AstrOsWifiMessageType::CONNECT:
      {
        ESP_LOGI(TAG, "Connecting to wifi network");
        char ssid[33];
        std::string pass = "";

        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
          lv_dropdown_get_selected_str(ui_settingsscreen_cbxssids, ssid, 33);
          auto txt = lv_textarea_get_text(ui_settingsscreen_txtpassword);
          pass = txt;

          xSemaphoreGive(xGuiSemaphore);
        }
        wifiController.Connect(ssid, pass);
        break;
      }
      case AstrOsWifiMessageType::DISCONNECT:
        ESP_LOGI(TAG, "Disconnecting from wifi network");
        break;
      default:
        ESP_LOGI(TAG, "Unknown message type");
        break;
      }

      free(msg.message);
    }
  }
}

void uiUpdateTask(void *arg)
{
  auto queue = (QueueHandle_t)arg;
  astros_ui_message_t msg;
  while (1)
  {
    if (xQueueReceive(queue, &msg, pdMS_TO_TICKS(500)) == pdTRUE)
    {
      auto highWaterMark = uxTaskGetStackHighWaterMark(NULL);
      if (highWaterMark < 500)
      {
        ESP_LOGW(TAG, "UI Update Task Stack HWM: %d", highWaterMark);
      }

      ESP_LOGI(TAG, "Received message: %d", (int)msg.type);
      switch (msg.type)
      {
      case AstrOsUiMessageType::WIFI_SCAN_COMPLETED:
      {
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
          lv_obj_add_flag(ui_settingsscreen_spnwifi, LV_OBJ_FLAG_HIDDEN);
          lv_obj_clear_flag(ui_settingsscreen_btnwifiscan, LV_OBJ_FLAG_HIDDEN);
          xSemaphoreGive(xGuiSemaphore);
        }
        break;
      }
      case AstrOsUiMessageType::SSID_DETECTED:
      {
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, pdMS_TO_TICKS(500)))
        {
          lv_dropdown_add_option(ui_settingsscreen_cbxssids, msg.message, LV_DROPDOWN_POS_LAST);
          xSemaphoreGive(xGuiSemaphore);
        }
        break;
      }
      case AstrOsUiMessageType::WIFI_CONNECTING:
      {
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
          lv_obj_set_style_bg_color(ui_settingsscreen_btnwificonnect, lv_color_make(255, 192, 0), 0);
          lv_obj_set_style_bg_color(ui_mainscreen_btnwifi, lv_color_make(255, 192, 0), 0);
          xSemaphoreGive(xGuiSemaphore);
        }
        break;
      }
      case AstrOsUiMessageType::WIFI_CONNECTED:
      {
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
          lv_obj_set_style_bg_color(ui_settingsscreen_btnwificonnect, lv_color_make(143, 206, 0), 0);
          lv_obj_set_style_bg_color(ui_mainscreen_btnwifi, lv_color_make(143, 206, 0), 0);
          xSemaphoreGive(xGuiSemaphore);
        }
        break;
      }
      case AstrOsUiMessageType::WIFI_DISCONNECTED:
      {
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
          lv_obj_set_style_bg_color(ui_settingsscreen_btnwificonnect, lv_color_make(184, 28, 28), 0);
          lv_obj_set_style_bg_color(ui_mainscreen_btnwifi, lv_color_make(184, 28, 28), 0);
          xSemaphoreGive(xGuiSemaphore);
        }
        break;
      }
      case AstrOsUiMessageType::MODAL_MESSAGE:
      {
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
          lv_label_set_text(ui_settingsscreen_settingsmodal, msg.message);
          lv_label_set_text(ui_mainscreen_mainmodal, msg.message);
          lv_obj_clear_flag(ui_mainscreen_mainmodal, LV_OBJ_FLAG_HIDDEN);
          lv_obj_clear_flag(ui_settingsscreen_settingsmodal, LV_OBJ_FLAG_HIDDEN);
          xSemaphoreGive(xGuiSemaphore);
        }
        break;
      }
      default:
        ESP_LOGI(TAG, "Unknown message type");
        break;
      }

      free(msg.message);
    }
  }
}

//========== UI Methods ==========

void onScript1(lv_event_t *e) {
  astros_ui_message_t msg;
  msg.type = AstrOsUiMessageType::MODAL_MESSAGE;
  msg.message = strdup("Script 1");
  if (xQueueSend(uiQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
  {
    free(msg.message);
  }
}
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
  lv_dropdown_clear_options(ui_settingsscreen_cbxssids);

  astros_wifi_message_t msg;
  msg.type = AstrOsWifiMessageType::SCAN_START;
  msg.message = nullptr;
  if (xQueueSend(wifiQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
  {
    free(msg.message);
  }
}

void onWifiConnect(lv_event_t *e)
{
  astros_wifi_message_t msg;
  msg.type = AstrOsWifiMessageType::CONNECT;
  msg.message = nullptr;
  if (xQueueSend(wifiQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
  {
    free(msg.message);
  }
}
void onSyncScripts(lv_event_t *e) {}

void onCbxSSIDChanged(lv_event_t *e)
{
  auto *cbx = lv_event_get_target(e);
  lv_dropdown_set_text(cbx, NULL);
  xSemaphoreGive(xGuiSemaphore);
}

void onKbdPressed(lv_event_t *e)
{
  auto *kbd = lv_event_get_target(e);
  auto btn_id = lv_keyboard_get_selected_btn(kbd);

  // 39 - check
  // 22 - enter
  // 35 - kbd
  if (btn_id == 22)
  {
    // connect

    lv_obj_clear_state(ui_settingsscreen_txtpassword, LV_STATE_FOCUSED);
    lv_obj_add_flag(kbd, LV_OBJ_FLAG_HIDDEN);
  }
  else if (btn_id == 39 || btn_id == 35)
  {
    lv_obj_clear_state(ui_settingsscreen_txtpassword, LV_STATE_FOCUSED);
    lv_obj_add_flag(kbd, LV_OBJ_FLAG_HIDDEN);
  }
}

void onModalClick(lv_event_t *e)
{
  lv_obj_add_flag(ui_mainscreen_mainmodal, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ui_settingsscreen_settingsmodal, LV_OBJ_FLAG_HIDDEN);
}