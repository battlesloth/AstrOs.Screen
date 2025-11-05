#ifndef _ASTROSSCREEN_STORAGE_NVS_MANAGER_H
#define _ASTROSSCREEN_STORAGE_NVS_MANAGER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

    typedef struct
    {
        char ssid[33];
        char password[65];
    } svc_config_t;

    // save the service configuration to NVS
    bool nvsSaveServiceConfig(svc_config_t config);

    // load the service configuration from NVS
    bool nvsLoadServiceConfig(svc_config_t *config);

    // clear the service configuration from NVS
    bool nvsClearServiceConfig();

    // save api key to NVS
    bool nvsSaveApiKey(const char *apiKey);

    // load api key from NVS
    bool nvsLoadApiKey(char *apiKey);

    // save host to NVS
    bool nvsSaveHost(const char *host);

    // load host from NVS
    bool nvsLoadHost(char *host);

    // save use gateway to NVS
    bool nvsSaveUseGateway(bool useGateway);

    // load use gateway from NVS
    bool nvsLoadUseGateway();

#ifdef __cplusplus
}
#endif

#endif // _ASTROSSCREEN_STORAGE_NVS_MANAGER_H