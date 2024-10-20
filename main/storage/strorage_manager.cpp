#include "storage_manager.hpp"

#include <esp_vfs.h>
#include <esp_vfs_fat.h>
#include <vfs_fat_internal.h>
#include <nvs_flash.h>
#include <sdmmc_cmd.h>
#include "../utility/astros_util.h"

#define MOUNT_POINT "/sdcard"

#define PIN_NUM_MISO 13
#define PIN_NUM_MOSI 11
#define PIN_NUM_CLK 12 
#define PIN_NUM_CS 10    

static const char *TAG = "StorageManager";

static sdmmc_card_t *card;

AstrOsStorageManager storageManager;

AstrOsStorageManager::AstrOsStorageManager() {}
AstrOsStorageManager::~AstrOsStorageManager()
{

    const char mountPoint[] = MOUNT_POINT;

    // All done, unmount partition and disable SPI peripheral
    esp_vfs_fat_sdcard_unmount(mountPoint, card);
    ESP_LOGI(TAG, "Card unmounted");

    // deinitialize the bus after all devices are removed
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_host_device_t device = static_cast<spi_host_device_t>(host.slot);
    spi_bus_free(device);
}

esp_err_t AstrOsStorageManager::Init()
{

    ESP_LOGI(TAG, "Initalizing Flash");

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_LOGI(TAG, "Erasing flash");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    else
    {
        if (logError(TAG, __FUNCTION__, __LINE__, err))
        {
            return err;
        }
    }

    ESP_LOGI(TAG, "Mounting SD Card");

    return this->mountSdCard();
}

bool AstrOsStorageManager::saveServiceConfig(svc_config_t config)
{
    return nvsSaveServiceConfig(config);
}

bool AstrOsStorageManager::loadServiceConfig(svc_config_t *config)
{
    return nvsLoadServiceConfig(config);
}

bool AstrOsStorageManager::clearServiceConfig()
{
    return nvsClearServiceConfig();
}


bool AstrOsStorageManager::saveApiKey(const char *apiKey)
{
    return nvsSaveApiKey(apiKey);
}

bool AstrOsStorageManager::loadApiKey(char *apiKey)
{
    return nvsLoadApiKey(apiKey);
}

/**************************************************************
 * SD Card methods
 ***************************************************************/

bool AstrOsStorageManager::formatSdCard()
{
    char drv[3] = {'0', ':', 0};
    const size_t workbuf_size = 4096;
    void *workbuf = NULL;
    ESP_LOGI(TAG, "Formatting the SD card");

    size_t allocation_unit_size = 16 * 1024;

    workbuf = ff_memalloc(workbuf_size);
    if (workbuf == NULL)
    {
        ESP_LOGE(TAG, "Error formatting SD card: ESP_ERR_NO_MEM");
        return false;
    }

    size_t alloc_unit_size = esp_vfs_fat_get_allocation_unit_size(
        card->csd.sector_size,
        allocation_unit_size);

    MKFS_PARM param;

    param.fmt = FM_ANY;
    param.au_size = alloc_unit_size;

    FRESULT res = f_mkfs(drv, &param, workbuf, workbuf_size);
    if (res != FR_OK)
    {
        ESP_LOGE(TAG, "Error formatting SD card: f_mkfs failed (%d)", res);
        return false;
    }

    free(workbuf);

    mkdir("/sdcard/scripts", 0777);

    ESP_LOGI(TAG, "Successfully formatted the SD card");

    return true;
}

esp_err_t AstrOsStorageManager::mountSdCard()
{
    esp_err_t err;

    esp_vfs_fat_mount_config_t mountConfig;

    mountConfig.format_if_mount_failed = true;
    mountConfig.max_files = 5;
    mountConfig.allocation_unit_size = 16 * 1024;

    const char mountPoint[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initalizing SD card");

    ESP_LOGI(TAG, "Using SPI peripheral");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();

    auto dma_chan = SPI_DMA_CH_AUTO;
    auto mosi = PIN_NUM_MOSI;
    auto miso = PIN_NUM_MISO;
    auto sclk = PIN_NUM_CLK;
    auto cs = PIN_NUM_CS;

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = mosi,
        .miso_io_num = miso,
        .sclk_io_num = sclk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000};

    spi_host_device_t device = static_cast<spi_host_device_t>(host.slot);

    err = spi_bus_initialize(device, &bus_cfg, dma_chan);
    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return err;
    }

    sdspi_device_config_t slotConfig = SDSPI_DEVICE_CONFIG_DEFAULT();
    slotConfig.gpio_cs = static_cast<gpio_num_t>(cs);
    slotConfig.host_id = device;

    ESP_LOGI(TAG, "Mounting file system");

    err = esp_vfs_fat_sdspi_mount(mountPoint, &host, &slotConfig, &mountConfig, &card);

    if (logError(TAG, __FUNCTION__, __LINE__, err))
    {
        ESP_LOGE(TAG, "Failed to mount file system.");
        return err;
    }

    ESP_LOGI(TAG, "FIle system mounted");

    sdmmc_card_print_info(stdout, card);

    return err;
}

bool AstrOsStorageManager::saveFile(std::string filename, std::string data)
{
    FILE *fd = NULL;

    std::string path = AstrOsStorageManager::setFilePath(filename);

    ESP_LOGI(TAG, "Saving %s", path.c_str());

    if (access(path.c_str(), F_OK) == 0)
    {
        ESP_LOGI(TAG, "File %s exists already. Deleting...", path.c_str());
        unlink(path.c_str());
    }

    fd = fopen(path.c_str(), "w");
    if (!fd)
    {
        ESP_LOGE(TAG, "Failed to create file : %s", path.c_str());
        return false;
    }

    fwrite(data.c_str(), sizeof(char), strlen(data.c_str()) + 1, fd);

    fclose(fd);

    ESP_LOGI(TAG, "Saved %s", path.c_str());

    return true;
}

bool AstrOsStorageManager::deleteFile(std::string filename)
{

    std::string path = AstrOsStorageManager::setFilePath(filename);

    if (access(path.c_str(), F_OK) == 0)
    {
        ESP_LOGI(TAG, "File %s exists already. Deleting...", path.c_str());
        unlink(path.c_str());
    }

    return true;
}

bool AstrOsStorageManager::fileExists(std::string filename)
{
    std::string path = AstrOsStorageManager::setFilePath(filename);

    return access(path.c_str(), F_OK) == 0;
}

std::string AstrOsStorageManager::readFile(std::string filename)
{
    std::string path = AstrOsStorageManager::setFilePath(filename);

    if (access(path.c_str(), F_OK) != 0)
    {
        ESP_LOGE(TAG, "File does not exist: %s", path.c_str());
        return "error";
    }

    FILE *f = fopen(path.c_str(), "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open %s for reading", path.c_str());
        return "error";
    }

    std::string result = "";

    char segment[100];

    while (fgets(segment, sizeof(segment), f))
    {
        result.append(segment);
    }

    fclose(f);

    return result;
}

std::vector<std::string> AstrOsStorageManager::listFiles(std::string folder)
{
    std::vector<std::string> result;

    const char *entrytype;
    struct dirent *entry;
    struct stat entry_stat;

    std::string entryPath = AstrOsStorageManager::setFilePath(folder);

    DIR *dir = opendir(entryPath.c_str());

    if (!dir)
    {
        ESP_LOGE(TAG, "Failed to open dir %s", entryPath.c_str());
        return result;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        entrytype = (entry->d_type == DT_DIR ? "directory" : "file");

        const char *ep = entryPath.c_str();

        if (stat(ep, &entry_stat) == -1)
        {
            ESP_LOGE(TAG, "Failed to stat %s : %s", entrytype, entry->d_name);
            continue;
        }

        result.push_back(entry->d_name);
    }

    closedir(dir);

    return result;
}

/**************************************************************
 * Utility methods
 ***************************************************************/

std::string AstrOsStorageManager::setFilePath(std::string filename)
{

    std::string out = MOUNT_POINT + std::string("/") + filename;

    return out;
}