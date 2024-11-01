#ifndef __WIFICONTROLLER_HPP
#define __WIFICONTROLLER_HPP

// needed for QueueHandle_t, must be in this order
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <esp_wifi.h>
#include <vector>
#include <string>

class WifiController
{
private:
    QueueHandle_t uiUpdateQueue;
    void (*wifiScanCallback)(void);
    int retryCount = 0;

public:
    WifiController(/* args */);
    ~WifiController();
    void Init(QueueHandle_t queue);
    void Scan();
    bool Connect(std::string ssid, std::string password);
    void Disconnect();
    void OnConnected(char *gateway);
    void OnConnecting();
    void OnDisconnected();
    int GetRetries();
    void IncrementRetries();
};

extern WifiController wifiController;

#endif //__WIFICONTROLLER_H