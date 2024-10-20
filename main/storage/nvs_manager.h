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
        char ip[16];
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

#ifdef __cplusplus
}
#endif

#endif // _ASTROSSCREEN_STORAGE_NVS_MANAGER_H