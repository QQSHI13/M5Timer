#include "storage.h"

void Storage::begin() {
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    
    // Open NVS namespace
    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvsHandle);
    if (err != ESP_OK) {
        Serial.printf("Error opening NVS: %s\n", esp_err_to_name(err));
        nvsInitialized = false;
        return;
    }
    
    nvsInitialized = true;
    Serial.println("NVS storage initialized");
}

TimerSettings Storage::load() {
    TimerSettings settings;
    
    if (!nvsInitialized) {
        Serial.println("NVS not initialized, using defaults");
        return settings;
    }
    
    StoredSettings data;
    size_t length = sizeof(data);
    
    esp_err_t err = nvs_get_blob(nvsHandle, KEY_SETTINGS, &data, &length);
    
    bool valid = false;
    if (err == ESP_OK && length == sizeof(data)) {
        // Validate magic number and checksum
        if (data.magic == SETTINGS_MAGIC && 
            data.checksum == calculateChecksum(data) &&
            data.version == 1) {
            valid = true;
        }
    }
    
    if (valid) {
        settings.workMinutes = data.workMinutes;
        settings.breakMinutes = data.breakMinutes;
        settings.longBreakMinutes = data.longBreakMinutes;
        settings.workSessionsBeforeLongBreak = data.workSessionsBeforeLongBreak;
        settings.soundEnabled = data.soundEnabled != 0;
        
        Serial.println("Settings loaded from NVS");
    } else {
        // Invalid data, use defaults
        Serial.println("NVS invalid or empty, using defaults");
        settings = TimerSettings();
        save(settings);
    }
    
    return settings;
}

void Storage::save(const TimerSettings& settings) {
    if (!nvsInitialized) {
        Serial.println("NVS not initialized, cannot save");
        return;
    }
    
    StoredSettings data;
    data.magic = SETTINGS_MAGIC;
    data.version = 1;
    data.workMinutes = constrain(settings.workMinutes, 1, 60);
    data.breakMinutes = constrain(settings.breakMinutes, 1, 30);
    data.longBreakMinutes = constrain(settings.longBreakMinutes, 5, 60);
    data.workSessionsBeforeLongBreak = constrain(settings.workSessionsBeforeLongBreak, 2, 10);
    data.soundEnabled = settings.soundEnabled ? 1 : 0;
    data.reserved[0] = 0;
    data.reserved[1] = 0;
    data.checksum = calculateChecksum(data);
    
    esp_err_t err = nvs_set_blob(nvsHandle, KEY_SETTINGS, &data, sizeof(data));
    if (err != ESP_OK) {
        Serial.printf("Error saving settings: %s\n", esp_err_to_name(err));
        return;
    }
    
    err = nvs_commit(nvsHandle);
    if (err != ESP_OK) {
        Serial.printf("Error committing NVS: %s\n", esp_err_to_name(err));
        return;
    }
    
    Serial.println("Settings saved to NVS");
}

void Storage::resetToDefaults() {
    TimerSettings defaults;
    save(defaults);
}

bool Storage::loadCompletionFlag() {
    if (!nvsInitialized) {
        return false;
    }
    
    CompletionFlagData data;
    size_t length = sizeof(data);
    
    esp_err_t err = nvs_get_blob(nvsHandle, KEY_COMPLETION, &data, &length);
    
    if (err != ESP_OK || length != sizeof(data)) {
        return false;
    }
    
    // Validate checksum
    if (data.checksum != calculateCompletionChecksum(data)) {
        return false;
    }
    
    return (data.magic == COMPLETION_MAGIC_VALID);
}

void Storage::saveCompletionFlag(bool completed) {
    if (!nvsInitialized) {
        return;
    }
    
    CompletionFlagData data;
    data.magic = completed ? COMPLETION_MAGIC_VALID : COMPLETION_MAGIC_CLEAR;
    data.flag = completed ? 1 : 0;
    data.checksum = calculateCompletionChecksum(data);
    
    esp_err_t err = nvs_set_blob(nvsHandle, KEY_COMPLETION, &data, sizeof(data));
    if (err != ESP_OK) {
        return;
    }
    
    nvs_commit(nvsHandle);
}

uint32_t Storage::calculateChecksum(const StoredSettings& data) {
    uint32_t checksum = 0;
    checksum ^= data.magic;
    checksum += data.version;
    checksum += data.workMinutes;
    checksum += data.breakMinutes;
    checksum += data.longBreakMinutes;
    checksum += data.workSessionsBeforeLongBreak;
    checksum += data.soundEnabled;
    return checksum;
}

uint32_t Storage::calculateCompletionChecksum(const CompletionFlagData& data) {
    uint32_t checksum = 0;
    checksum ^= data.magic;
    checksum += data.flag;
    return checksum;
}
