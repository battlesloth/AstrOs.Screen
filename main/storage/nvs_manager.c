#include "nvs_manager.h"

#include <stdbool.h>
#include <nvs_flash.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "../utility/astros_util.h"

static const char *TAG = "NvsManager";

// Helper function to clean strings by removing non-printable characters
static void cleanString(char *str) {
    char *src = str;
    char *dst = str;
    
    while (*src) {
        if (isprint((unsigned char)*src) && *src != '\t' && *src != '\n' && *src != '\r') {
            *dst = *src;
            dst++;
        }
        src++;
    }
    *dst = '\0';
}

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
        strcpy(config->ssid, "error");
        result = false;
    }
    else
    {
        // Clean the SSID by removing non-printable characters
        cleanString(config->ssid);
    }

    defaultSize = 65;
    err = nvs_get_str(nvsHandle, "password", config->password, &defaultSize);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        strcpy(config->ssid, "error");
        result = false;
    }
    else
    {
        // Clean the password by removing non-printable characters
        cleanString(config->password);
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

    err = nvs_erase_key(nvsHandle, "host");
    logError(TAG, __FUNCTION__, __LINE__, err);

    err = nvs_erase_key(nvsHandle, "useGateway");
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

bool nvsSaveApiKey(const char *apiKey)
{
    esp_err_t err;
    nvs_handle_t nvsHandle;

    err = nvs_open("config", NVS_READWRITE, &nvsHandle);

    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }

    err = nvs_set_str(nvsHandle, "apikey", apiKey);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }

    nvs_close(nvsHandle);

    return true;
}

bool nvsLoadApiKey(char *apiKey)
{
    esp_err_t err;
    nvs_handle_t nvsHandle;
    size_t defaultSize = 0;

    err = nvs_open("config", NVS_READWRITE, &nvsHandle);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }

    defaultSize = 65;
    err = nvs_get_str(nvsHandle, "apikey", apiKey, &defaultSize);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        strcpy(apiKey, "error");
        nvs_close(nvsHandle);
        return false;
    }

    // Clean the API key by removing non-printable characters
    cleanString(apiKey);

    nvs_close(nvsHandle);

    return true;
}

bool nvsSaveHost(const char *host)
{
    esp_err_t err;
    nvs_handle_t nvsHandle;

    err = nvs_open("config", NVS_READWRITE, &nvsHandle);

    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }

    err = nvs_set_str(nvsHandle, "host", host);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }

    nvs_close(nvsHandle);

    return true;
}

bool nvsLoadHost(char *host)
{
    esp_err_t err;
    nvs_handle_t nvsHandle;
    size_t defaultSize = 16;

    err = nvs_open("config", NVS_READWRITE, &nvsHandle);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }

    err = nvs_get_str(nvsHandle, "host", host, &defaultSize);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        strcpy(host, "\0");
        nvs_close(nvsHandle);
        return false;
    }

    // Clean the host string by removing non-printable characters
    cleanString(host);

    nvs_close(nvsHandle);

    return true;
}

bool nvsSaveUseGateway(bool useGateway)
{
    esp_err_t err;
    nvs_handle_t nvsHandle;

    err = nvs_open("config", NVS_READWRITE, &nvsHandle);

    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }

    err = nvs_set_u8(nvsHandle, "useGateway", useGateway);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }

    nvs_close(nvsHandle);

    return true;
}

bool nvsLoadUseGateway()
{
    esp_err_t err;
    nvs_handle_t nvsHandle;
    uint8_t value = 0;

    err = nvs_open("config", NVS_READWRITE, &nvsHandle);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }

    err = nvs_get_u8(nvsHandle, "useGateway", &value);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        nvs_close(nvsHandle);
        return false;
    }
    nvs_close(nvsHandle);

    return value > 0;
}