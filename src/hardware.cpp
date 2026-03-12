#include "hardware.h"
#include "config.h"
#include <M5Unified.h>
#include <I2C_BM8563.h>

extern I2C_BM8563 rtc;

void setupPower() {
    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, HIGH);
    
    // Initialize M5Unified for power management
    auto cfg = M5.config();
    M5.begin(cfg);
}

void setupRTC() {
    Wire1.begin(8, 10);  // SDA=GPIO8, SCL=GPIO10
    rtc.begin();
}
