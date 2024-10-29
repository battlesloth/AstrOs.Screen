#ifndef __HTTP_CLIENT_HPP
#define __HTTP_CLIENT_HPP

#include <esp_err.h>
#include <esp_http_client.h>
#include <string>

class AstrOsHttpClient
{
private:
    std::string apiKey;
    std::string host;
public:
    AstrOsHttpClient();
    ~AstrOsHttpClient();
    void Init(QueueHandle_t queue);

    static QueueHandle_t queue;

    void SetApiKey(std::string apiKey);
    void SetHost(std::string host);

    void SendSyncRequest();
    void SendScriptCommand(std::string scriptId);
    void SendPanicStop();
};

extern AstrOsHttpClient httpClient;


#endif