#pragma once

#include "config.h"

// Non-blocking buzzer

class Buzzer {
public:
    void begin();
    void beep(int durationMs = 100, int frequency = 1000);
    void playAlarm();
    void playClick();
    void playDoubleClick();
    void playSyncTone();
    void stop();
    void update();
    bool isPlaying() const { return playing; }
    
private:
    bool playing = false;
    unsigned long toneEndTime = 0;
    int alarmPhase = -1;
    unsigned long alarmNextPhaseTime = 0;
};
