#pragma once

#include "config.h"

class BatteryMonitor {
public:
    void begin();
    void update();  // Call periodically
    
    float getVoltage() const;
    uint8_t getPercentage() const;
    bool isLow() const;
    bool isCritical() const;
    // Note: isCharging() removed - no charging circuit in minimal hardware
    
    // Get battery status for LED indication
    enum class Status { OK, LOW_BATTERY, CRITICAL };  // CHARGING removed
    Status getStatus() const;
    
private:
    static constexpr float VOLTAGE_DIVIDER_RATIO = 2.0f;  // Voltage divider on AtomS3
    static constexpr float ADC_REF_VOLTAGE = 3.3f;
    static constexpr int ADC_RESOLUTION = 4095;
    
    // LiPo battery thresholds (3.7V nominal)
    static constexpr float VOLTAGE_MAX = 4.2f;
    static constexpr float VOLTAGE_LOW = 3.5f;      // Warning threshold
    static constexpr float VOLTAGE_CRITICAL = 3.3f; // Deep sleep to protect battery
    
    float lastVoltage = 0;
    unsigned long lastReadTime = 0;
    static constexpr unsigned long READ_INTERVAL_MS = 30000; // Read every 30 seconds
    
    float readRawVoltage();
};