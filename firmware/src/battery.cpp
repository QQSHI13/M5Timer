#include "battery.h"

void BatteryMonitor::begin() {
    // AtomS3 Lite has internal voltage divider on GPIO6
    pinMode(BATTERY_PIN, INPUT);
    analogReadResolution(12);  // 12-bit ADC (0-4095)
    analogSetAttenuation(ADC_11db);  // Full voltage range
    
    // Initial reading
    lastVoltage = readRawVoltage();
    lastReadTime = millis();
    
    Serial.printf("Battery: %.2fV (%d%%)\n", lastVoltage, getPercentage());
}

void BatteryMonitor::update() {
    unsigned long now = millis();
    if (now - lastReadTime >= READ_INTERVAL_MS) {
        lastReadTime = now;
        lastVoltage = readRawVoltage();
        
        if (isCritical()) {
            Serial.println("WARNING: Battery critically low! Entering protective sleep...");
        } else if (isLow()) {
            Serial.printf("WARNING: Battery low (%.2fV)\n", lastVoltage);
        }
    }
}

float BatteryMonitor::getVoltage() const {
    return lastVoltage;
}

uint8_t BatteryMonitor::getPercentage() const {
    if (lastVoltage >= VOLTAGE_MAX) return 100;
    if (lastVoltage <= VOLTAGE_CRITICAL) return 0;
    
    // Linear interpolation between critical and max
    float pct = (lastVoltage - VOLTAGE_CRITICAL) / (VOLTAGE_MAX - VOLTAGE_CRITICAL) * 100.0f;
    return static_cast<uint8_t>(constrain(pct, 0, 100));
}

bool BatteryMonitor::isLow() const {
    return lastVoltage < VOLTAGE_LOW && lastVoltage > VOLTAGE_CRITICAL;
}

bool BatteryMonitor::isCritical() const {
    return lastVoltage <= VOLTAGE_CRITICAL;
}

// Note: isCharging() removed - minimal hardware has no charging circuit

BatteryMonitor::Status BatteryMonitor::getStatus() const {
    // CHARGING status removed - no charging circuit in minimal hardware
    if (isCritical()) return Status::CRITICAL;
    if (isLow()) return Status::LOW_BATTERY;
    return Status::OK;
}

float BatteryMonitor::readRawVoltage() {
    // Wait for ADC to stabilize before reading (ESP32-S3 ADC needs time after startup)
    delay(10);
    
    // Read ADC and convert to voltage
    int raw = analogRead(BATTERY_PIN);
    float adcVoltage = (float)raw / ADC_RESOLUTION * ADC_REF_VOLTAGE;
    float batteryVoltage = adcVoltage * VOLTAGE_DIVIDER_RATIO;
    
    // Simple averaging (read twice for stability)
    delayMicroseconds(100);
    raw = analogRead(BATTERY_PIN);
    adcVoltage = (float)raw / ADC_RESOLUTION * ADC_REF_VOLTAGE;
    batteryVoltage = (batteryVoltage + adcVoltage * VOLTAGE_DIVIDER_RATIO) / 2.0f;
    
    return batteryVoltage;
}