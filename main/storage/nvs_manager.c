#include "nvs_manager.h"

#include <stdbool.h>
#include <nvs_flash.h>
#include <string.h>
#include <math.h>

#include "../utility/astros_util.h"

static const char *TAG = "NvsManager";

bool nvsSaveServiceConfig(svc_config_t config)
{

    nvs_handle_t nvsHandle;
    esp_err_t err;

    err = nvs_open("config", NVS_READWRITE, &nvsHandle);

    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }

    err = nvs_set_str(nvsHandle, "ssid", config.ssid);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }

    err = nvs_set_str(nvsHandle, "password", config.password);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }


    err = nvs_set_str(nvsHandle, "ip", config.password);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }
    
    nvs_close(nvsHandle);

    return true;
}

bool nvsLoadServiceConfig(svc_config_t *config)
{
    esp_err_t err;
    nvs_handle_t nvsHandle;
    size_t defaultSize = 0;
    bool result = true;

    err = nvs_open("config", NVS_READWRITE, &nvsHandle);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }

    defaultSize = 33;
    err = nvs_get_str(nvsHandle, "ssid", config->ssid, &defaultSize);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        memset(config->ssid, "error\0", 6);
        result = false;
    }

    defaultSize = 65;
    err = nvs_get_str(nvsHandle, "password", config->password, &defaultSize);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        memset(config->ssid, "error\0", 6);
        result = false;
    }

    defaultSize = 16;
    err = nvs_get_str(nvsHandle, "ip", config->ip, &defaultSize);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        memcpy(config->ip, "error\0", 6);
        result = false;
    }

    nvs_close(nvsHandle);

    return result;
}

bool nvsClearServiceConfig()
{
    esp_err_t err;
    nvs_handle_t nvsHandle;

    err = nvs_open("config", NVS_READWRITE, &nvsHandle);

    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }

    err = nvs_erase_key(nvsHandle, "ssid");
    logError(TAG, __FUNCTION__, __LINE__, err);

    err = nvs_erase_key(nvsHandle, "password");
    logError(TAG, __FUNCTION__, __LINE__, err);

    err = nvs_erase_key(nvsHandle, "ip");
    logError(TAG, __FUNCTION__, __LINE__, err);

    err = nvs_commit(nvsHandle);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }

    nvs_close(nvsHandle);
    return true;
}