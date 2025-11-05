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
#include "httpclient/astros_http_client.hpp"
#include "utility/astros_string_util.hpp"

#define TAG "AstrOs Screen"
#define QUEUE_LENGTH 10

SemaphoreHandle_t xGuiSemaphore;
bool isWifiConnected = false;
bool sdCardMounted = false;

//========== Queues ===========
static QueueHandle_t uiQueue;
static QueueHandle_t wifiQueue;

//========== Tasks ===========
void lvglTask(void *arg);
void uiUpdateTask(void *arg);
void wifiTask(void *arg);

//========== Timers ==========
static esp_timer_handle_t uiInitTimer;
static esp_timer_handle_t clearLoadingScreenTimer;

//========== Methods ==========
void uiInitCallback(void *arg);
void clearLoadingScreenCallback(void *arg);
void setButtonNames();

extern "C"
{
  void app_main(void);
}

void app_main()
{
  // start screen
  WZ8048C050_Init();
  ui_init();
  lv_timer_handler();

  xGuiSemaphore = xSemaphoreCreateMutex();
  wifiQueue = xQueueCreate(QUEUE_LENGTH, sizeof(astros_wifi_message_t));
  uiQueue = xQueueCreate(QUEUE_LENGTH, sizeof(astros_ui_message_t));

  ESP_ERROR_CHECK(storageManager.Init());

  // Load WIFI creds
  svc_config_t svcConfig;

  if (storageManager.loadServiceConfig(&svcConfig))
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

      auto creds = std::string(svcConfig.ssid) + ":" + svcConfig.password;
      msg.message = (char *)malloc(creds.size() + 1);
      memcpy(msg.message, creds.c_str(), creds.size());
      msg.message[creds.size()] = '\0';

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

  // load api key
  char apiKey[65] = "";
  if (storageManager.loadApiKey(apiKey))
  {
    httpClient.SetApiKey(apiKey);

    if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
    {
      lv_textarea_set_text(ui_settingsscreen_txtapikey, apiKey);
      xSemaphoreGive(xGuiSemaphore);
    }
  }

  // load use gateway
  if (storageManager.loadUseGateway())
  {
    httpClient.SetUseHost(true);
    if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
    {
      lv_obj_add_state(ui_settingsscreen_chkusegateway, LV_STATE_CHECKED);
      lv_obj_clear_state(ui_settingsscreen_txtserverip, LV_STATE_DISABLED);
      xSemaphoreGive(xGuiSemaphore);
    }
  }
  else
  {
    httpClient.SetUseHost(false);
    if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
    {
      lv_obj_clear_state(ui_settingsscreen_chkusegateway, LV_STATE_CHECKED);
      lv_obj_add_state(ui_settingsscreen_txtserverip, LV_STATE_DISABLED);
      xSemaphoreGive(xGuiSemaphore);
    }
  }

  // load host
  char host[16] = "";
  if (storageManager.loadHost(host))
  {
    httpClient.SetHost(host);
    if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
    {
      lv_textarea_set_text(ui_settingsscreen_txtserverip, host);
      xSemaphoreGive(xGuiSemaphore);
    }
  }

  if (storageManager.fileExists("scripts.json"))
  {
    auto scriptBlob = storageManager.readFile("scripts.json");
    ESP_LOGI(TAG, "Scripts file found: %s", scriptBlob.c_str());
    script.LoadScript(scriptBlob);
  }
  else
  {
    ESP_LOGI(TAG, "Scripts file not found");
    script.DefaultScript();
  }

  httpClient.Init(uiQueue, apiKey);
  wifiController.Init(uiQueue);

  xTaskCreate(&wifiTask, "wifiTask", 8192, (void *)wifiQueue, 9, NULL);
  xTaskCreate(&uiUpdateTask, "uiUpdateTask", 4096, (void *)uiQueue, 8, NULL);
  xTaskCreate(&lvglTask, "lvglTask", 4096, NULL, 10, NULL);

  const esp_timer_create_args_t uiInitTimerArgs = {
      .callback = &uiInitCallback,
      .arg = NULL,
      .dispatch_method = ESP_TIMER_TASK,
      .name = "delay_ui_update"};

  ESP_ERROR_CHECK(esp_timer_create(&uiInitTimerArgs, &uiInitTimer));
  ESP_ERROR_CHECK(esp_timer_start_once(uiInitTimer, 5 * 1000));
}

//========== Callbacks ==========

void uiInitCallback(void *arg)
{
  ESP_LOGI(TAG, "UI Init Callback");
  astros_ui_message_t msg;
  msg.type = AstrOsUiMessageType::UPDATE_BUTTON_NAMES;
  msg.message = nullptr;

  if (xQueueSend(uiQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
  {
    free(msg.message);
  }

  const esp_timer_create_args_t clearLoadingScreenTimerArgs = {
      .callback = &clearLoadingScreenCallback,
      .arg = NULL,
      .dispatch_method = ESP_TIMER_TASK,
      .name = "clear_loading_screen"};

  ESP_ERROR_CHECK(esp_timer_create(&clearLoadingScreenTimerArgs, &clearLoadingScreenTimer));
  ESP_ERROR_CHECK(esp_timer_start_once(clearLoadingScreenTimer, 3 * 1000 * 1000));
}

void clearLoadingScreenCallback(void *arg)
{
  astros_ui_message_t msg;
  msg.type = AstrOsUiMessageType::CLEAR_LOADING_SCREEN;
  msg.message = nullptr;

  if (xQueueSend(uiQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
  {
    free(msg.message);
  }
}

//========== Tasks ==========
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

      ESP_LOGI(TAG, "Received wifi message: %d", (int)msg.type);
      switch (msg.type)
      {
      case AstrOsWifiMessageType::SCAN_START:
        wifiController.Scan();
        break;
      case AstrOsWifiMessageType::CONNECT:
      {
        ESP_LOGI(TAG, "Connecting to wifi network");

        auto creds = std::string(msg.message);
        auto split = AstrOsStringUtils::splitString(creds, ':');

        auto ssid = split[0];
        auto pass = split[1];

        svc_config_t svcConfig;

        memcpy(svcConfig.ssid, ssid.c_str(), 33);
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
      case AstrOsWifiMessageType::SYNC_SCRIPTS:
      {
        ESP_LOGI(TAG, "Syncing scripts");
        httpClient.SendSyncRequest();
        break;
      }
      case AstrOsWifiMessageType::SCRIPT_COMMAND:
      {
        ESP_LOGI(TAG, "Sending script command");
        httpClient.SendScriptCommand(msg.message);
        break;
      }
      case AstrOsWifiMessageType::PANIC_STOP:
      {
        ESP_LOGI(TAG, "Sending panic stop");
        httpClient.SendPanicStop();
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

      ESP_LOGI(TAG, "Received UI message: %d", (int)msg.type);
      switch (msg.type)
      {
      case AstrOsUiMessageType::CLEAR_LOADING_SCREEN:
      {

        auto sdCardMounted = storageManager.validateSdCard();
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
          lv_scr_load_anim(ui_mainscreen, LV_SCR_LOAD_ANIM_FADE_OUT, 500, 0, true);
          xSemaphoreGive(xGuiSemaphore);
        }

        if (sdCardMounted)
        {
          ESP_LOGI(TAG, "SD Card mounted successfully");
        }
        else
        {
          ESP_LOGI(TAG, "No SD Card detected");
          // pop modal message
          if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
          {
            lv_label_set_text(ui_settingsscreen_lblerrormessage, "SD Card error");
            lv_obj_clear_flag(ui_settingsscreen_settingserrormodal, LV_OBJ_FLAG_HIDDEN);

            lv_label_set_text(ui_mainscreen_lblerrormessage, "SD Card error");
            lv_obj_clear_flag(ui_mainscreen_mainerrormodal, LV_OBJ_FLAG_HIDDEN);

            xSemaphoreGive(xGuiSemaphore);
          }
        }
        break;
      }
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
        httpClient.SetGateway(msg.message);
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
          isWifiConnected = true;
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
          isWifiConnected = false;
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
      case AstrOsUiMessageType::SCRIPTS_RECEIVED:
      {
        if (storageManager.saveFile("scripts.json", msg.message))
        {
          script.LoadScript(msg.message);
          setButtonNames();
        }
        else
        {
          if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
          {
            lv_label_set_text(ui_settingsscreen_lblerrormessage, "Failed to save scripts");
            lv_obj_clear_flag(ui_settingsscreen_settingserrormodal, LV_OBJ_FLAG_HIDDEN);

            lv_label_set_text(ui_mainscreen_lblerrormessage, "Failed to save scripts");
            lv_obj_clear_flag(ui_mainscreen_mainerrormodal, LV_OBJ_FLAG_HIDDEN);

            xSemaphoreGive(xGuiSemaphore);
          }
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
void setButtonNameFromString(lv_obj_t *btnLabel, std::string name)
{
  if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
  {
    // lvgl calls free on the value passed so we need to copy it
    auto len = strlen(name.c_str());
    char val[len + 1];
    memcpy(val, name.c_str(), len);
    val[len] = '\0';

    lv_label_set_text(btnLabel, val);
    xSemaphoreGive(xGuiSemaphore);
  }
}

void setButtonNames()
{
  ESP_LOGI("AstrosScript", "Setting button names: Page %d", script.GetPageCount());

  for (int i = 1; i <= 9; i++)
  {
    ESP_LOGI("AstrosScript", "  Button %d: %s", i, script.GetScriptName(i).c_str());
  }

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

void sendNotConnectedMessage()
{
  astros_ui_message_t msg;
  msg.type = AstrOsUiMessageType::MODAL_MESSAGE;
  msg.message = (char *)malloc(24);
  memcpy(msg.message, "Not connected to wifi", 24);

  if (xQueueSend(uiQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
  {
    free(msg.message);
  }
}

void sendScriptToQueue(std::string scriptId)
{

  if (!isWifiConnected)
  {
    sendNotConnectedMessage();
    return;
  }

  if (scriptId.empty() || scriptId == "0")
  {
    return;
  }

  astros_wifi_message_t msg;
  msg.type = AstrOsWifiMessageType::SCRIPT_COMMAND;

  msg.message = (char *)malloc(scriptId.size() + 1);
  memcpy(msg.message, scriptId.c_str(), scriptId.size());
  msg.message[scriptId.size()] = '\0';

  if (xQueueSend(wifiQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
  {
    free(msg.message);
  }
}

void onScript1(lv_event_t *e)
{
  sendScriptToQueue(script.GetScriptCommand(1));
}
void onScript2(lv_event_t *e)
{
  sendScriptToQueue(script.GetScriptCommand(2));
}
void onScript3(lv_event_t *e)
{
  sendScriptToQueue(script.GetScriptCommand(3));
}
void onScript4(lv_event_t *e)
{
  sendScriptToQueue(script.GetScriptCommand(4));
}
void onScript5(lv_event_t *e)
{
  sendScriptToQueue(script.GetScriptCommand(5));
}
void onScript6(lv_event_t *e)
{
  sendScriptToQueue(script.GetScriptCommand(6));
}
void onScript7(lv_event_t *e)
{
  sendScriptToQueue(script.GetScriptCommand(7));
}
void onScript8(lv_event_t *e)
{
  sendScriptToQueue(script.GetScriptCommand(8));
}
void onScript9(lv_event_t *e)
{
  sendScriptToQueue(script.GetScriptCommand(9));
}
void onPanicStop(lv_event_t *e)
{

  if (!isWifiConnected)
  {
    sendNotConnectedMessage();
    return;
  }

  astros_wifi_message_t msg;
  msg.type = AstrOsWifiMessageType::PANIC_STOP;
  msg.message = nullptr;

  if (xQueueSend(wifiQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
  {
    free(msg.message);
  }
}

void onBack(lv_event_t *e)
{
  script.DecrementPage();

  astros_ui_message_t msg;
  msg.type = AstrOsUiMessageType::UPDATE_BUTTON_NAMES;
  msg.message = nullptr;

  if (xQueueSend(uiQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
  {
    free(msg.message);
  }
}

void onForward(lv_event_t *e)
{
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
  char ssid[33];

  lv_dropdown_get_selected_str(ui_settingsscreen_cbxssids, ssid, 33);
  auto pass = lv_textarea_get_text(ui_settingsscreen_txtpassword);

  std::string creds = std::string(ssid) + ":" + pass;

  astros_wifi_message_t msg;
  msg.type = AstrOsWifiMessageType::CONNECT;
  msg.message = (char *)malloc(creds.size() + 1);
  memcpy(msg.message, creds.c_str(), creds.size());
  msg.message[creds.size()] = '\0';

  if (xQueueSend(wifiQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
  {
    free(msg.message);
  }
}

// when txt area loses focus, save the value
void onApiKeyLostFocus(lv_event_t *e)
{
  auto txt = lv_textarea_get_text(ui_settingsscreen_txtapikey);
  httpClient.SetApiKey(txt);
  storageManager.saveApiKey(txt);
}

void onIpKeyLostFocus(lv_event_t *e)
{
  auto txt = lv_textarea_get_text(ui_settingsscreen_txtserverip);
  httpClient.SetHost(txt);
  storageManager.saveHost(txt);
}

void onUseGatewayChecked(lv_event_t *e)
{
  httpClient.SetUseHost(true);
  storageManager.saveUseGateway(true);
}

void onUseGatewayUnchecked(lv_event_t *e)
{
  httpClient.SetUseHost(false);
  storageManager.saveUseGateway(false);
}

void onSyncScripts(lv_event_t *e)
{
  if (!isWifiConnected)
  {
    sendNotConnectedMessage();
    return;
  }

  astros_wifi_message_t msg;

  msg.type = AstrOsWifiMessageType::SYNC_SCRIPTS;
  msg.message = nullptr;

  if (xQueueSend(wifiQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
  {
    free(msg.message);
  }
}

void onCbxSSIDChanged(lv_event_t *e)
{
  auto cbx = lv_event_get_target(e);
  lv_dropdown_set_text(cbx, NULL);
}

void onSettingsClosed(lv_event_t *e)
{
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