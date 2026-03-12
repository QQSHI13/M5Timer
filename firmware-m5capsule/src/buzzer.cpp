#include "buzzer.h"

void Buzzer::begin() {
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    playing = false;
}

void Buzzer::beep(int durationMs, int frequency) {
    ledcAttachPin(BUZZER_PIN, 0);
    ledcWriteTone(0, frequency);
    toneEndTime = millis() + durationMs;
    playing = true;
}

void Buzzer::stop() {
    ledcWriteTone(0, 0);
    ledcDetachPin(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, LOW);
    playing = false;
}

void Buzzer::playClick() {
    beep(50, 1000);
}

void Buzzer::playDoubleClick() {
    beep(50, 1000);
}

void Buzzer::playAlarm() {
    beep(200, 1000);
}

void Buzzer::playSyncTone() {
    beep(150, 1500);
}

void Buzzer::update() {
    if (!playing) return;
    
    if (millis() >= toneEndTime) {
        stop();
    }
}
