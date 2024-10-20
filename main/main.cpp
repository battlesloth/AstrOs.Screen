#include "esp_log.h"
#include <WZ8048C050.h>
#include "lvgl.h"
#include <nvs_flash.h>

#include "ui/ui.h"
#include "ui/ui_events.h"

#include "messaging/astros_messaging.hpp"
#include "wifi_controller/wifi_controller.hpp"
#include "storage/storage_manager.hpp"
#include "scripts/astros_script.hpp"

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

//========== Methods ==========
void setButtonNames();
void setButtonNameFromString(lv_obj_t *btn, std::string name);

extern "C"
{
  void app_main(void);
}

void app_main()
{
  xGuiSemaphore = xSemaphoreCreateMutex();
  wifiQueue = xQueueCreate(QUEUE_LENGTH, sizeof(astros_wifi_message_t));
  uiQueue = xQueueCreate(QUEUE_LENGTH, sizeof(astros_ui_message_t));

  ESP_ERROR_CHECK(storageManager.Init());

  svc_config_t svcConfig;
  auto cfigLoaded(storageManager.loadServiceConfig(&svcConfig));

  // load api key, if it exists, else create empty string
  char apiKey[65] = "";
  storageManager.loadApiKey(apiKey);

  WZ8048C050_Init();
  ui_init();
  lv_timer_handler();

  if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
  {
    lv_textarea_set_text(ui_settingsscreen_txtapikey, apiKey);
    xSemaphoreGive(xGuiSemaphore);
  }

  if (cfigLoaded)
  {
    ESP_LOGI(TAG, "Service Config Loaded");
    if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
    {
      lv_dropdown_add_option(ui_settingsscreen_cbxssids, svcConfig.ssid, 0);
      lv_dropdown_set_selected(ui_settingsscreen_cbxssids, 0);
      lv_dropdown_set_text(ui_settingsscreen_cbxssids, NULL);
      lv_textarea_set_text(ui_settingsscreen_txtpassword, svcConfig.password);
      xSemaphoreGive(xGuiSemaphore);
    }

    if (svcConfig.ssid[0] != '\0')
    {
      astros_wifi_message_t msg;
      msg.type = AstrOsWifiMessageType::CONNECT;
      msg.message = nullptr;
      if (xQueueSend(wifiQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
      {
        free(msg.message);
      }
    }
  }
  else
  {
    ESP_LOGI(TAG, "Service Config Not Loaded");
  }

  if (storageManager.fileExists("scripts.json"))
  {
    auto scriptBlob = storageManager.readFile("scripts.json");
    script.LoadScript(scriptBlob);
  }
  else
  {
    ESP_LOGI(TAG, "Scripts file not found");
  }

  setButtonNames();

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

          ESP_LOGI(TAG, "PWD: %s", txt);

          pass = txt;

          xSemaphoreGive(xGuiSemaphore);
        }

        svc_config_t svcConfig;

        memcpy(svcConfig.ssid, ssid, 33);
        svcConfig.ssid[32] = '\0';

        auto passLen = pass.length() <= 64 ? pass.length() : 64;
        memcpy(svcConfig.password, pass.c_str(), passLen);
        svcConfig.password[passLen] = '\0';

        storageManager.saveServiceConfig(svcConfig);

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
          // lvgl calls free on the value passed so we need to copy it
          char val[strlen(msg.message) + 1];

          memcpy(val, msg.message, strlen(msg.message) + 1);

          lv_label_set_text(ui_settingsscreen_lblerrormessage, val);
          lv_obj_clear_flag(ui_settingsscreen_settingserrormodal, LV_OBJ_FLAG_HIDDEN);

          lv_label_set_text(ui_mainscreen_lblerrormessage, val);
          lv_obj_clear_flag(ui_mainscreen_mainerrormodal, LV_OBJ_FLAG_HIDDEN);

          xSemaphoreGive(xGuiSemaphore);
        }
        break;
      }
      case AstrOsUiMessageType::UPDATE_BUTTON_NAMES:
      {
        setButtonNames();
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

void setButtonNames()
{
  setButtonNameFromString(ui_mainscreen_lblscript1, script.GetScriptName(1));
  setButtonNameFromString(ui_mainscreen_lblscript2, script.GetScriptName(2));
  setButtonNameFromString(ui_mainscreen_lblscript3, script.GetScriptName(3));
  setButtonNameFromString(ui_mainscreen_lblscript4, script.GetScriptName(4));
  setButtonNameFromString(ui_mainscreen_lblscript5, script.GetScriptName(5));
  setButtonNameFromString(ui_mainscreen_lblscript6, script.GetScriptName(6));
  setButtonNameFromString(ui_mainscreen_lblscript7, script.GetScriptName(7));
  setButtonNameFromString(ui_mainscreen_lblscript8, script.GetScriptName(8));
  setButtonNameFromString(ui_mainscreen_lblscript9, script.GetScriptName(9));
}

void setButtonNameFromString(lv_obj_t *btnLabel, std::string name)
{
  if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
  {
    // lvgl calls free on the value passed so we need to copy it
    auto len = strlen(name.c_str()) + 1;
    char val[len];
    memcpy(val, name.c_str(), len);

    lv_label_set_text(btnLabel, val);
    xSemaphoreGive(xGuiSemaphore);
  }
}

void onScript1(lv_event_t *e)
{
  astros_ui_message_t msg;
  msg.type = AstrOsUiMessageType::MODAL_MESSAGE;

  std::string id = "script 1";

  msg.message = (char *)malloc(id.size() + 1); // dummy data
  memccpy(msg.message, id.c_str(), 0, id.size());
  msg.message[id.size()] = '\0';

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

void onBack(lv_event_t *e) {
  script.DecrementPage();

  astros_ui_message_t msg;
  msg.type = AstrOsUiMessageType::UPDATE_BUTTON_NAMES;
  msg.message = nullptr;

  if (xQueueSend(uiQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
  {
    free(msg.message);
  }
}

void onForward(lv_event_t *e) {
  script.IncrementPage();

  astros_ui_message_t msg;
  msg.type = AstrOsUiMessageType::UPDATE_BUTTON_NAMES;
  msg.message = nullptr;

  if (xQueueSend(uiQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
  {
    free(msg.message);
  }
}

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

// when txt area loses focus, save the value
void onApiKeyLostFocus(lv_event_t *e)
{
  auto txt = lv_textarea_get_text(ui_settingsscreen_txtapikey);
  storageManager.saveApiKey(txt);
}

void onSyncScripts(lv_event_t *e) {}

void onCbxSSIDChanged(lv_event_t *e)
{
  auto *cbx = lv_event_get_target(e);
  lv_dropdown_set_text(cbx, NULL);
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

void onErrorAck(lv_event_t *e)
{
  ESP_LOGI(TAG, "Modal Clicked");
  lv_obj_add_flag(ui_mainscreen_mainerrormodal, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ui_settingsscreen_settingserrormodal, LV_OBJ_FLAG_HIDDEN);
}