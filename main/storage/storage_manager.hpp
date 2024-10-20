#ifndef __ASTROSSCREEN_STORAGE_MANAGER_H
#define __ASTROSSCREEN_STORAGE_MANAGER_H

#include <esp_log.h>
#include <stdbool.h>
#include <string>
#include <vector>
#include <esp_err.h>
#include "nvs_manager.h"


class AstrOsStorageManager
{
private:
    esp_err_t mountSdCard();
    std::string setFilePath(std::string filename);
public:
    AstrOsStorageManager();
    ~AstrOsStorageManager();
    esp_err_t Init();

    bool loadServiceConfig(svc_config_t *config);
    bool saveServiceConfig(svc_config_t config);
    bool clearServiceConfig();

    bool saveApiKey(const char *apiKey);
    bool loadApiKey(char *apiKey);

    bool saveFile(std::string filename, std::string data);
    bool deleteFile(std::string filename);
    bool fileExists(std::string filename);
    std::string readFile(std::string filename);
    std::vector<std::string> listFiles(std::string folder);
   
    bool formatSdCard();
};

extern AstrOsStorageManager storageManager;

#endif // __ASTROSSCREEN_STORAGE_MANAGER_H