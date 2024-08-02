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
    bool saveFileSd(std::string filename, std::string data);
    bool deleteFileSd(std::string filename);
    bool fileExistsSd(std::string filename);
    std::string readFileSd(std::string filename);
    std::vector<std::string> listFilesSd(std::string folder);

public:
    AstrOsStorageManager();
    ~AstrOsStorageManager();
    esp_err_t Init();

    bool loadServiceConfig(svc_config_t *config);
    bool saveServiceConfig(svc_config_t config);
    bool clearServiceConfig();

    bool saveFile(std::string filename, std::string data);
    bool deleteFile(std::string filename);
    bool fileExists(std::string filename);

    std::string readFile(std::string filename);

    bool formatSdCard();
};

extern AstrOsStorageManager storageManager;

#endif // __ASTROSSCREEN_STORAGE_MANAGER_H