#include "storage.h"
#include <Preferences.h>

static const char* PREFS_NAMESPACE = "pomodoro";

// ============== SETTINGS IMPLEMENTATION ==============
void Settings::load(Preferences& prefs) {
    workMinutes = prefs.getUChar("workMin", 25);
    breakMinutes = prefs.getUChar("breakMin", 5);
    longBreakMinutes = prefs.getUChar("longBreakMin", 15);
    workSessionsBeforeLongBreak = prefs.getUChar("sessions", 4);
    soundEnabled = prefs.getBool("sound", true);
    ledBrightness = prefs.getUChar("ledBright", 16);
    buzzerVolume = prefs.getUChar("buzzVol", 24);
}

void Settings::save(Preferences& prefs) const {
    prefs.putUChar("workMin", workMinutes);
    prefs.putUChar("breakMin", breakMinutes);
    prefs.putUChar("longBreakMin", longBreakMinutes);
    prefs.putUChar("sessions", workSessionsBeforeLongBreak);
    prefs.putBool("sound", soundEnabled);
    prefs.putUChar("ledBright", ledBrightness);
    prefs.putUChar("buzzVol", buzzerVolume);
}

void Settings::reset() {
    workMinutes = 25;
    breakMinutes = 5;
    longBreakMinutes = 15;
    workSessionsBeforeLongBreak = 4;
    soundEnabled = true;
    ledBrightness = 16;
    buzzerVolume = 24;
}

String Settings::toString() const {
    return String("work=") + workMinutes +
           ",break=" + breakMinutes +
           ",longBreak=" + longBreakMinutes +
           ",sessions=" + workSessionsBeforeLongBreak +
           ",sound=" + (soundEnabled ? 1 : 0) +
           ",ledBright=" + ledBrightness +
           ",buzzVol=" + buzzerVolume;
}

bool Settings::fromString(const String& str) {
    int workIdx = str.indexOf("work=");
    int breakIdx = str.indexOf("break=");
    int longIdx = str.indexOf("longBreak=");
    int sessIdx = str.indexOf("sessions=");
    int soundIdx = str.indexOf("sound=");
    int ledIdx = str.indexOf("ledBright=");
    int volIdx = str.indexOf("buzzVol=");
    
    if (workIdx >= 0) {
        int endIdx = str.indexOf(',', workIdx);
        if (endIdx < 0) endIdx = str.length();
        int val = str.substring(workIdx + 5, endIdx).toInt();
        if (val >= 1 && val <= 60) workMinutes = val;
    }
    if (breakIdx >= 0) {
        int endIdx = str.indexOf(',', breakIdx);
        if (endIdx < 0) endIdx = str.length();
        int val = str.substring(breakIdx + 6, endIdx).toInt();
        if (val >= 1 && val <= 30) breakMinutes = val;
    }
    if (longIdx >= 0) {
        int endIdx = str.indexOf(',', longIdx);
        if (endIdx < 0) endIdx = str.length();
        int val = str.substring(longIdx + 10, endIdx).toInt();
        if (val >= 5 && val <= 60) longBreakMinutes = val;
    }
    if (sessIdx >= 0) {
        int endIdx = str.indexOf(',', sessIdx);
        if (endIdx < 0) endIdx = str.length();
        int val = str.substring(sessIdx + 9, endIdx).toInt();
        if (val >= 2 && val <= 10) workSessionsBeforeLongBreak = val;
    }
    if (soundIdx >= 0) {
        int endIdx = str.indexOf(',', soundIdx);
        if (endIdx < 0) endIdx = str.length();
        soundEnabled = str.substring(soundIdx + 6, endIdx).toInt() != 0;
    }
    if (ledIdx >= 0) {
        int endIdx = str.indexOf(',', ledIdx);
        if (endIdx < 0) endIdx = str.length();
        int val = str.substring(ledIdx + 10, endIdx).toInt();
        if (val >= 0 && val <= 255) ledBrightness = val;
    }
    if (volIdx >= 0) {
        int endIdx = str.indexOf(',', volIdx);
        if (endIdx < 0) endIdx = str.length();
        int val = str.substring(volIdx + 8, endIdx).toInt();
        if (val >= 0 && val <= 255) buzzerVolume = val;
    }
    
    return true;
}

// ============== TIMER STATE IMPLEMENTATION ==============
void TimerState::load(Preferences& prefs) {
    uint8_t modeVal = prefs.getUChar("tmode", 0);
    mode = static_cast<TimerMode>(modeVal);
    remainingSeconds = prefs.getInt("remSec", 25 * 60);
    isRunning = prefs.getBool("tRunning", false);
    completedWorkSessions = prefs.getUChar("compSessions", 0);
}

void TimerState::save(Preferences& prefs) const {
    prefs.putUChar("tmode", static_cast<uint8_t>(mode));
    prefs.putInt("remSec", remainingSeconds);
    prefs.putBool("tRunning", isRunning);
    prefs.putUChar("compSessions", completedWorkSessions);
}

int TimerState::getDurationMinutes(const Settings& settings) const {
    switch (mode) {
        case TimerMode::WORK: return settings.workMinutes;
        case TimerMode::BREAK: return settings.breakMinutes;
        case TimerMode::LONG_BREAK: return settings.longBreakMinutes;
        default: return settings.workMinutes;
    }
}

void TimerState::reset(const Settings& settings) {
    remainingSeconds = getDurationMinutes(settings) * 60;
    isRunning = true;
}

// ============== STORAGE API ==============
void loadSettings(Settings& settings) {
    Preferences prefs;
    if (prefs.begin(PREFS_NAMESPACE, false)) {
        settings.load(prefs);
        prefs.end();
    }
}

void saveSettings(const Settings& settings) {
    Preferences prefs;
    if (prefs.begin(PREFS_NAMESPACE, false)) {
        settings.save(prefs);
        prefs.end();
    }
}

void resetSettings(Settings& settings) {
    settings.reset();
    saveSettings(settings);
}

void loadTimerState(TimerState& timerState) {
    Preferences prefs;
    if (prefs.begin(PREFS_NAMESPACE, false)) {
        timerState.load(prefs);
        prefs.end();
    }
}

void saveTimerState(const TimerState& timerState) {
    Preferences prefs;
    if (prefs.begin(PREFS_NAMESPACE, false)) {
        timerState.save(prefs);
        prefs.end();
    }
}
