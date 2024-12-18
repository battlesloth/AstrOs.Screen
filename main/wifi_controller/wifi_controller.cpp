#include "wifi_controller.hpp"
#include <esp_wifi.h>
#include <esp_netif.h>

#include <messaging/astros_messaging.hpp>
#include <esp_log.h>
#include <string.h>
#include "../utility/astros_util.h"

#define TAG "AstrOs Wifi Controller"
static bool intentionalDisconnect = false;
static bool connected = false;

#define DEFAULT_SCAN_LIST_SIZE 20

WifiController wifiController;

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_START:
        {
            ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
            //esp_err_t err = esp_wifi_connect();
            //logError(TAG, __FUNCTION__, __LINE__, err);
            break;
        }
        case WIFI_EVENT_STA_CONNECTED:
        {
            ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
            connected = true;
            break;
        }
        case WIFI_EVENT_STA_DISCONNECTED:
        {
            wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t *)event_data;
            ESP_LOGI(TAG, "WIFI STA disconnected, Reason:%d.", event->reason);
            connected = false;
            if (intentionalDisconnect)
            {
                ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED - intentional disconnect");
                intentionalDisconnect = false;
                wifiController.OnDisconnected();
                return;
            }
            else if (wifiController.GetRetries() < 5)
            {
                ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
                esp_wifi_connect();
                wifiController.OnConnecting();
                wifiController.IncrementRetries();
            }
            else
            {
                ESP_LOGE(TAG, "Failed to connect to wifi network");
                wifiController.OnDisconnected();
                return;
            }
            break;
        }
        default:
            break;
        }
    }
    else if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
        case IP_EVENT_STA_GOT_IP:
        {
            ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
            esp_netif_ip_info_t ipInfo;
            esp_netif_get_ip_info(esp_netif_get_default_netif(), &ipInfo);
            char ipStr[IP4ADDR_STRLEN_MAX];
            wifiController.OnConnected(esp_ip4addr_ntoa(&ipInfo.gw, ipStr, IP4ADDR_STRLEN_MAX));
            break;
        }
        default:
            break;
        }
    }
}

WifiController::WifiController(/* args */)
{
}

WifiController::~WifiController()
{
}

void WifiController::Init(QueueHandle_t queue)
{
    this->uiUpdateQueue = queue;

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void WifiController::Scan()
{
    ESP_LOGI(TAG, "Scanning for wifi networks");
    if (connected)
    {
        this->Disconnect();
    }

    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
    uint16_t ap_count = 0;

    esp_wifi_scan_start(NULL, true);
    esp_wifi_scan_get_ap_num(&ap_count);
    esp_wifi_scan_get_ap_records(&number, ap_info);

    ESP_LOGI(TAG, "Found %d access points", ap_count);

    for (int i = 0; i < ap_count; i++)
    {
        ESP_LOGI(TAG, "SSID: %s, Secuity: %d", ap_info[i].ssid, ap_info[i].authmode);

        if (ap_info[i].authmode < 2)
        {
            ESP_LOGI(TAG, "Skipping network due to securty level");
            continue;
        }

        auto ssidSize = strlen((char *)ap_info[i].ssid) + 1;
        astros_ui_message_t msg;
        msg.type = AstrOsUiMessageType::SSID_DETECTED;
        msg.message = (char *)malloc(ssidSize);
        memcpy(msg.message, (char *)ap_info[i].ssid, ssidSize);

        if (xQueueSend(this->uiUpdateQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
        {
            ESP_LOGE(TAG, "Failed to send message to UI update queue");
            free(msg.message);
        }
    }

    astros_ui_message_t msg;
    msg.type = AstrOsUiMessageType::WIFI_SCAN_COMPLETED;
    msg.message = nullptr;
    if (xQueueSend(this->uiUpdateQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to send message to UI update queue");
    }
}

bool WifiController::Connect(std::string ssid, std::string password)
{
    ESP_LOGI(TAG, "Connecting to wifi network %s, pwd_len:%d", ssid.c_str(), password.length());

    if (connected)
    {
        this->Disconnect();
    }
    
    this->retryCount = 0;
    error_t err = ESP_OK;

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            .password = "",
            .threshold = {
                .authmode = WIFI_AUTH_WPA_PSK,
                },
        },
    };

    memcpy(wifi_config.sta.ssid, ssid.c_str(), ssid.length() < 32 ? ssid.length() : 32);
    memcpy(wifi_config.sta.password, password.c_str(), password.length() < 64 ? password.length() : 64);

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        return false;
    }
    err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        return false;
    }
    err = esp_wifi_connect();
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        return false;
    }

    return true;
}

void WifiController::Disconnect()
{
    ESP_LOGI(TAG, "Disconnecting from wifi network");
    intentionalDisconnect = true;
    esp_err_t err = esp_wifi_disconnect();
    logError(TAG, __FUNCTION__, __LINE__, err);
}

int WifiController::GetRetries()
{
    return this->retryCount;
}

void WifiController::IncrementRetries()
{
    this->retryCount++;
}

void WifiController::OnConnecting()
{
    astros_ui_message_t msg;
    msg.type = AstrOsUiMessageType::WIFI_CONNECTING;
    msg.message = nullptr;
    if (xQueueSend(this->uiUpdateQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to send message to UI update queue");
    }
}

void WifiController::OnConnected(char *gateway)
{
    astros_ui_message_t msg;
    msg.type = AstrOsUiMessageType::WIFI_CONNECTED;
   
    auto len = strlen(gateway);
    msg.message = (char *)malloc(len + 1);
    memcpy(msg.message, gateway, len);
    msg.message[len] = '\0';
   
    if (xQueueSend(this->uiUpdateQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to send message to UI update queue");
    }
}

void WifiController::OnDisconnected()
{
    astros_ui_message_t msg;
    msg.type = AstrOsUiMessageType::WIFI_DISCONNECTED;
    msg.message = nullptr;
    if (xQueueSend(this->uiUpdateQueue, &msg, pdMS_TO_TICKS(500)) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to send message to UI update queue");
    }
}