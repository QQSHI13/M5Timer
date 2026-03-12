#include "storage.h"
#include <EEPROM.h>

void Storage::begin() {
    EEPROM.begin(EEPROM_SIZE);
}

TimerSettings Storage::load() {
    TimerSettings settings;
    
    StoredData data;
    EEPROM.get(SETTINGS_ADDR, data);
    
    // Validate magic number and checksum
    bool valid = false;
    if ((data.magic == MAGIC || data.magic == MAGIC_V2) && 
        data.checksum == calculateChecksum(data)) {
        // Additional validation: check struct version
        if (validateStructVersion(data.structVersion)) {
            valid = true;
        }
    }
    
    if (valid) {
        settings.workMinutes = data.workMinutes;
        settings.breakMinutes = data.breakMinutes;
        settings.longBreakMinutes = data.longBreakMinutes;
        settings.workSessionsBeforeLongBreak = data.workSessionsBeforeLongBreak;
        settings.soundEnabled = data.soundEnabled;
        
        // If old format (MAGIC v1 without explicit structVersion), use defaults for new fields
        if (data.magic == MAGIC || data.structVersion < 2) {
            settings.longBreakMinutes = 15;
            settings.workSessionsBeforeLongBreak = 4;
        }
    } else {
        // Invalid data, use defaults
        Serial.println("EEPROM invalid, using defaults");
        settings = TimerSettings();
        save(settings);
    }
    
    return settings;
}

void Storage::save(const TimerSettings& settings) {
    StoredData data;
    data.magic = MAGIC_V2;
    data.structVersion = STRUCT_VERSION;
    data.workMinutes = constrain(settings.workMinutes, 1, 60);
    data.breakMinutes = constrain(settings.breakMinutes, 1, 30);
    data.longBreakMinutes = constrain(settings.longBreakMinutes, 5, 60);
    data.workSessionsBeforeLongBreak = constrain(settings.workSessionsBeforeLongBreak, 2, 10);
    data.soundEnabled = settings.soundEnabled ? 1 : 0;
    data.checksum = calculateChecksum(data);
    
    EEPROM.put(SETTINGS_ADDR, data);
    EEPROM.commit();
}

void Storage::resetToDefaults() {
    TimerSettings defaults;
    save(defaults);
}

bool Storage::loadCompletionFlag() {
    CompletionFlagData data;
    EEPROM.get(COMPLETION_FLAG_ADDR, data);
    
    // Validate magic and checksum
    if (data.checksum != calculateCompletionChecksum(data)) {
        return false;  // Invalid/corrupted data
    }
    
    return (data.magic == COMPLETION_MAGIC_VALID);
}

void Storage::saveCompletionFlag(bool completed) {
    CompletionFlagData data;
    data.magic = completed ? COMPLETION_MAGIC_VALID : COMPLETION_MAGIC_CLEAR;
    data.reserved = 0;
    data.checksum = calculateCompletionChecksum(data);
    
    EEPROM.put(COMPLETION_FLAG_ADDR, data);
    EEPROM.commit();
}

uint8_t Storage::calculateChecksum(const StoredData& data) {
    uint8_t checksum = 0;
    checksum ^= (data.magic >> 24) & 0xFF;
    checksum ^= (data.magic >> 16) & 0xFF;
    checksum ^= (data.magic >> 8) & 0xFF;
    checksum ^= data.magic & 0xFF;
    checksum ^= data.structVersion;
    checksum ^= data.workMinutes;
    checksum ^= data.breakMinutes;
    checksum ^= data.longBreakMinutes;
    checksum ^= data.workSessionsBeforeLongBreak;
    checksum ^= data.soundEnabled;
    return checksum;
}

uint8_t Storage::calculateCompletionChecksum(const CompletionFlagData& data) {
    uint8_t checksum = 0;
    checksum ^= data.magic;
    checksum ^= data.reserved;
    return checksum;
}

bool Storage::validateStructVersion(uint8_t version) {
    // Accept version 0 as legacy (before structVersion field was added)
    // Version 0 means magic-only validation (backward compatible)
    if (version == 0) {
        return true;
    }
    return (version >= MIN_SUPPORTED_VERSION && version <= MAX_SUPPORTED_VERSION);
}
