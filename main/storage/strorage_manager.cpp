#include "storage_manager.hpp"

#include <esp_vfs.h>
#include <esp_vfs_fat.h>
#include <vfs_fat_internal.h>
#include <nvs_flash.h>
#include <sdmmc_cmd.h>
#include <sys/stat.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>
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

bool AstrOsStorageManager::saveHost(const char *host)
{
    return nvsSaveHost(host);
}

bool AstrOsStorageManager::loadHost(char *host)
{
    return nvsLoadHost(host);
}

bool AstrOsStorageManager::saveUseGateway(bool useGateway)
{
    return nvsSaveUseGateway(useGateway);
}

bool AstrOsStorageManager::loadUseGateway()
{ 
    return nvsLoadUseGateway();
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

    esp_vfs_fat_mount_config_t mountConfig = {0};

    mountConfig.format_if_mount_failed = true;  
    mountConfig.max_files = 10;  
    mountConfig.allocation_unit_size = 16 * 1024;
    mountConfig.disk_status_check_enable = false; 

    const char mountPoint[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initalizing SD card");

    ESP_LOGI(TAG, "Using SPI peripheral");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    
    host.flags = SDMMC_HOST_FLAG_SPI;
    host.max_freq_khz = SDMMC_FREQ_DEFAULT;

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

    ESP_LOGI(TAG, "File system mounted");

    sdmmc_card_print_info(stdout, card);
    
    // Log card type to help diagnose SDIO vs SD card issues
    ESP_LOGI(TAG, "Card type: %s", 
             (card->is_sdio) ? "SDIO" : 
             (card->is_mmc) ? "MMC" : "SD");
    
    if (card->is_sdio) {
        ESP_LOGW(TAG, "Warning: Card detected as SDIO, but expecting SD card.");
    }

    return err;
}

bool AstrOsStorageManager::saveFile(std::string filename, std::string data)
{
    FILE *fd = NULL;

    std::string path = AstrOsStorageManager::setFilePath(filename);

    ESP_LOGI(TAG, "Attempting to save file: %s (%zu bytes)", path.c_str(), data.length());

    // Check if SD card mount point exists and is accessible
    struct stat mount_stat;
    if (stat(MOUNT_POINT, &mount_stat) != 0) {
        ESP_LOGE(TAG, "SD card mount point %s does not exist or is not accessible: errno %d (%s)", 
                 MOUNT_POINT, errno, strerror(errno));
        return false;
    }

    if (!S_ISDIR(mount_stat.st_mode)) {
        ESP_LOGE(TAG, "Mount point %s is not a directory", MOUNT_POINT);
        return false;
    }

    // Check if directory exists and create it if necessary
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash != std::string::npos) {
        std::string dirPath = path.substr(0, lastSlash);
        
        struct stat st;
        if (stat(dirPath.c_str(), &st) != 0) {
            // Directory doesn't exist, create it
            ESP_LOGI(TAG, "Creating directory: %s", dirPath.c_str());
            if (mkdir(dirPath.c_str(), 0777) != 0) {
                ESP_LOGE(TAG, "Failed to create directory: %s, errno: %d (%s)", 
                         dirPath.c_str(), errno, strerror(errno));
                return false;
            }
            ESP_LOGI(TAG, "Directory created successfully: %s", dirPath.c_str());
        } else if (!S_ISDIR(st.st_mode)) {
            ESP_LOGE(TAG, "Path exists but is not a directory: %s", dirPath.c_str());
            return false;
        } else {
            ESP_LOGI(TAG, "Directory already exists: %s", dirPath.c_str());
        }
    }

    if (access(path.c_str(), F_OK) == 0)
    {
        ESP_LOGI(TAG, "File %s exists already. Deleting...", path.c_str());
        if (unlink(path.c_str()) != 0) {
            ESP_LOGE(TAG, "Failed to delete existing file: %s, errno: %d (%s)", 
                     path.c_str(), errno, strerror(errno));
            return false;
        }
    }

    fd = fopen(path.c_str(), "w");
    if (!fd)
    {
        ESP_LOGE(TAG, "Failed to create file: %s, errno: %d (%s)", 
                 path.c_str(), errno, strerror(errno));
        return false;
    }

    ESP_LOGI(TAG, "Writing %zu bytes to file", data.length());
    size_t written = fwrite(data.c_str(), sizeof(char), data.length(), fd);
    
    if (written != data.length()) {
        ESP_LOGE(TAG, "Failed to write complete data. Expected: %zu, Written: %zu, errno: %d (%s)", 
                 data.length(), written, errno, strerror(errno));
        fclose(fd);
        return false;
    }

    if (fclose(fd) != 0) {
        ESP_LOGE(TAG, "Failed to close file: %s, errno: %d (%s)", 
                 path.c_str(), errno, strerror(errno));
        return false;
    }

    ESP_LOGI(TAG, "Successfully saved %s (%zu bytes)", path.c_str(), data.length());

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

bool AstrOsStorageManager::validateSdCard()
{
    ESP_LOGI(TAG, "Validating SD card");
    
    // Check basic filesystem status
    if (!this->checkFilesystemStatus()) {
        ESP_LOGE(TAG, "Filesystem status check failed");
        return false;
    }
    
    // Check detailed mount status
    if (!this->checkMountStatus()) {
        ESP_LOGE(TAG, "Mount status check failed");
        return false;
    }
    
    // Test basic file operations - this is the real test
    if (!this->testBasicFileOperations()) {
        ESP_LOGW(TAG, "Basic file operations failed - trying to format SD card");
        
        if (this->formatSdCard()) {
            ESP_LOGI(TAG, "SD card formatted successfully, retesting operations");
            if (this->testBasicFileOperations()) {
                ESP_LOGI(TAG, "File operations successful after formatting");
                return true;
            } else {
                ESP_LOGE(TAG, "File operations still failed after formatting");
                return false;
            }
        } else {
            ESP_LOGE(TAG, "Failed to format SD card");
            return false;
        }
    }
    
    ESP_LOGI(TAG, "SD card validation successful");
    return true;
}

/**************************************************************
 * Utility methods
 ***************************************************************/

bool AstrOsStorageManager::checkFilesystemStatus()
{
    ESP_LOGI(TAG, "Checking filesystem status");
    
    // Check if mount point exists and is accessible
    struct stat mount_stat;
    if (stat(MOUNT_POINT, &mount_stat) != 0) {
        ESP_LOGE(TAG, "Mount point %s is not accessible: errno %d (%s)", 
                 MOUNT_POINT, errno, strerror(errno));
        return false;
    }
    
    // Check if it's a directory
    if (!S_ISDIR(mount_stat.st_mode)) {
        ESP_LOGE(TAG, "Mount point %s is not a directory", MOUNT_POINT);
        return false;
    }
    
    ESP_LOGI(TAG, "Mount point is accessible and is a directory");
    
    return true;
}

bool AstrOsStorageManager::checkMountStatus()
{
    ESP_LOGI(TAG, "Checking detailed mount status");
    
    // Check if the card variable is still valid
    if (!card) {
        ESP_LOGE(TAG, "SD card handle is NULL - card not properly mounted");
        return false;
    }
    
    // Print detailed card information
    ESP_LOGI(TAG, "Card name: %s", card->cid.name);
    ESP_LOGI(TAG, "Card type: %s", 
             (card->is_sdio) ? "SDIO" : 
             (card->is_mmc) ? "MMC" : "SD");
    ESP_LOGI(TAG, "Card capacity: %llu MB", ((uint64_t)card->csd.capacity) / (1024 * 1024));
    ESP_LOGI(TAG, "Card max frequency: %d kHz", card->max_freq_khz);
    
    // Check filesystem type and mount flags
    FILE* mounts = fopen("/proc/mounts", "r");
    if (mounts) {
        char line[256];
        bool found_sdcard = false;
        while (fgets(line, sizeof(line), mounts)) {
            if (strstr(line, "/sdcard")) {
                ESP_LOGI(TAG, "Mount info: %s", line);
                found_sdcard = true;
                
                // Check if mounted read-only
                if (strstr(line, "ro,") || strstr(line, "ro ")) {
                    ESP_LOGE(TAG, "SD card is mounted READ-ONLY!");
                    fclose(mounts);
                    return false;
                }
            }
        }
        fclose(mounts);
        
        if (!found_sdcard) {
            ESP_LOGW(TAG, "Could not find /sdcard in mount table");
        }
    } else {
        ESP_LOGW(TAG, "Could not open /proc/mounts (this is normal on some systems)");
    }
    
    // Try to check access permissions directly
    if (access(MOUNT_POINT, W_OK) != 0) {
        ESP_LOGW(TAG, "No write access to %s: errno %d (%s) - filesystem may need formatting", 
                 MOUNT_POINT, errno, strerror(errno));
        // Don't return false immediately - let the caller try formatting
        ESP_LOGW(TAG, "Continuing with file operations test...");
    } else {
        ESP_LOGI(TAG, "Write access check passed");
    }
    
    ESP_LOGI(TAG, "Mount status check completed");
    return true;
}

bool AstrOsStorageManager::testBasicFileOperations()
{
    ESP_LOGI(TAG, "Testing basic file operations");
    
    // Test 1: Simple file creation in root
    const char* test_filename = "basic_test.txt";
    const char* test_data = "Hello World";
    
    std::string test_path = std::string(MOUNT_POINT) + "/" + test_filename;
    
    ESP_LOGI(TAG, "Test 1: Creating file %s", test_path.c_str());
    
    FILE* test_file = fopen(test_path.c_str(), "w");
    if (!test_file) {
        ESP_LOGE(TAG, "Test 1 FAILED: Cannot create file: errno %d (%s)", errno, strerror(errno));
        return false;
    }
    
    size_t written = fwrite(test_data, 1, strlen(test_data), test_file);
    if (written != strlen(test_data)) {
        ESP_LOGE(TAG, "Test 1 FAILED: Write incomplete. Expected %zu, got %zu", strlen(test_data), written);
        fclose(test_file);
        return false;
    }
    
    if (fclose(test_file) != 0) {
        ESP_LOGE(TAG, "Test 1 FAILED: Cannot close file: errno %d (%s)", errno, strerror(errno));
        return false;
    }
    
    ESP_LOGI(TAG, "Test 1 PASSED: File created and written successfully");
    
    // Test 2: Read the file back
    ESP_LOGI(TAG, "Test 2: Reading file back");
    
    test_file = fopen(test_path.c_str(), "r");
    if (!test_file) {
        ESP_LOGE(TAG, "Test 2 FAILED: Cannot open file for reading: errno %d (%s)", errno, strerror(errno));
        return false;
    }
    
    char read_buffer[100] = {0};
    size_t read_count = fread(read_buffer, 1, sizeof(read_buffer)-1, test_file);
    fclose(test_file);
    
    if (strcmp(read_buffer, test_data) != 0) {
        ESP_LOGE(TAG, "Test 2 FAILED: Read data mismatch. Expected '%s', got '%s'", test_data, read_buffer);
        return false;
    }
    
    ESP_LOGI(TAG, "Test 2 PASSED: File read successfully");
    
    // Test 3: Delete the file
    ESP_LOGI(TAG, "Test 3: Deleting file");
    
    if (unlink(test_path.c_str()) != 0) {
        ESP_LOGE(TAG, "Test 3 FAILED: Cannot delete file: errno %d (%s)", errno, strerror(errno));
        return false;
    }
    
    ESP_LOGI(TAG, "Test 3 PASSED: File deleted successfully");
    ESP_LOGI(TAG, "All basic file operation tests PASSED");
    
    return true;
}

std::string AstrOsStorageManager::setFilePath(std::string filename)
{
    std::string out = MOUNT_POINT + std::string("/") + filename;

    return out;
}