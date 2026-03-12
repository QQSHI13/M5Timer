#pragma once

#include "config.h"
#include <nvs.h>
#include <nvs_flash.h>

class Storage {
public:
    void begin();
    TimerSettings load();
    void save(const TimerSettings& settings);
    void resetToDefaults();
    
    // Completion flag persistence across deep sleep
    bool loadCompletionFlag();
    void saveCompletionFlag(bool completed);
    
private:
    static constexpr const char* NVS_NAMESPACE = "pomodoro";
    static constexpr const char* KEY_SETTINGS = "settings";
    static constexpr const char* KEY_COMPLETION = "completion";
    
    // Magic numbers for data validation
    static constexpr uint32_t SETTINGS_MAGIC = 0x504D4433; // "PMD3" - M5Capsule version
    
    struct StoredSettings {
        uint32_t magic;
        uint8_t version;
        uint8_t workMinutes;
        uint8_t breakMinutes;
        uint8_t longBreakMinutes;
        uint8_t workSessionsBeforeLongBreak;
        uint8_t soundEnabled;
        uint8_t reserved[2];
        uint32_t checksum;
    };
    
    struct CompletionFlagData {
        uint32_t magic;
        uint32_t flag;
        uint32_t checksum;
    };
    
    static constexpr uint32_t COMPLETION_MAGIC_VALID = 0x434F4D50; // "COMP"
    static constexpr uint32_t COMPLETION_MAGIC_CLEAR = 0x434C5200; // "CLR"
    
    nvs_handle_t nvsHandle = 0;
    bool nvsInitialized = false;
    
    uint32_t calculateChecksum(const StoredSettings& data);
    uint32_t calculateCompletionChecksum(const CompletionFlagData& data);
};
