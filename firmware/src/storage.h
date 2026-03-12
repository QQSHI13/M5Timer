#pragma once

#include "config.h"

struct TimerSettings {
    uint8_t workMinutes = 25;
    uint8_t breakMinutes = 5;
    uint8_t longBreakMinutes = 15;
    uint8_t workSessionsBeforeLongBreak = 4;  // After 4 work sessions, take long break
    bool soundEnabled = true;
};

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
    static constexpr int EEPROM_SIZE = 128;  // Increased for additional data
    static constexpr int SETTINGS_ADDR = 0;
    static constexpr int COMPLETION_FLAG_ADDR = 64;  // Separate area for completion flag
    
    // Magic numbers for data validation
    static constexpr uint32_t MAGIC = 0x504D4454; // "PMDT"
    static constexpr uint32_t MAGIC_V2 = 0x504D4455; // "PMDU" - v2 with long break
    
    // Struct version for forward compatibility
    static constexpr uint8_t STRUCT_VERSION = 2;
    static constexpr uint8_t MIN_SUPPORTED_VERSION = 1;
    static constexpr uint8_t MAX_SUPPORTED_VERSION = 2;
    
    // Completion flag magic
    static constexpr uint8_t COMPLETION_MAGIC_VALID = 0xCC;  // Flag is valid
    static constexpr uint8_t COMPLETION_MAGIC_CLEAR = 0x00;  // Flag is cleared
    
    struct StoredData {
        uint32_t magic;
        uint8_t structVersion;  // Added for version validation
        uint8_t workMinutes;
        uint8_t breakMinutes;
        uint8_t longBreakMinutes;
        uint8_t workSessionsBeforeLongBreak;
        uint8_t soundEnabled;
        uint8_t checksum;
    };
    
    struct CompletionFlagData {
        uint8_t magic;      // COMPLETION_MAGIC_VALID or COMPLETION_MAGIC_CLEAR
        uint8_t reserved;   // Reserved for future use
        uint8_t checksum;
    };
    
    uint8_t calculateChecksum(const StoredData& data);
    uint8_t calculateCompletionChecksum(const CompletionFlagData& data);
    bool validateStructVersion(uint8_t version);
};
