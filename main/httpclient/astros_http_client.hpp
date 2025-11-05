#ifndef __HTTP_CLIENT_HPP
#define __HTTP_CLIENT_HPP

#include <esp_err.h>
#include <esp_http_client.h>
#include <string>

class AstrOsHttpClient
{
private:
    std::string apiKey;
    std::string gateway;
    std::string host;
    bool useHost;
    std::string getDestination();
public:
    AstrOsHttpClient();
    ~AstrOsHttpClient();
    void Init(QueueHandle_t queue, char * apiKey);

    static QueueHandle_t queue;

    void SetApiKey(const char * apiKey);
    void SetGateway(const char * host);
    void SetHost(const char * host);
    void SetUseHost(bool useHost);

    void SendSyncRequest();
    void SendScriptCommand(std::string scriptId);
    void SendPanicStop();
};

extern AstrOsHttpClient httpClient;


#endif