#include "buzzer.h"

static BuzzerState buzzerState;
static const Settings* settingsPtr = nullptr;
static uint8_t currentVolume = BUZZER_VOLUME;  // Configurable volume

void setupBuzzer() {
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    // Setup PWM channel 0 for buzzer (8-bit resolution, 2kHz default)
    ledcSetup(0, 2000, 8);
}

void setBuzzerSettings(const Settings& settings) {
    settingsPtr = &settings;
    currentVolume = settings.buzzerVolume;  // Update volume from settings
}

void setBuzzerVolume(uint8_t volume) {
    currentVolume = volume;
}

// Plays tones sequentially from queue (non-blocking)
void updateBuzzer() {
    if (!buzzerState.active) return;

    unsigned long now = millis();

    // Check if current tone is done
    if (now - buzzerState.toneStartTime >= buzzerState.currentTone.durationMs) {
        buzzerState.queueIndex++;

        if (buzzerState.queueIndex < buzzerState.queueSize) {
            // Play next tone in queue
            buzzerState.currentTone = buzzerState.queue[buzzerState.queueIndex];
            buzzerState.toneStartTime = now;
            ledcWriteTone(0, buzzerState.currentTone.frequency);
            ledcWrite(0, currentVolume);  // Apply configurable volume
        } else {
            // Queue finished
            ledcDetachPin(BUZZER_PIN);
            digitalWrite(BUZZER_PIN, LOW);
            buzzerState.active = false;
            buzzerState.queueSize = 0;
            buzzerState.queueIndex = 0;
        }
    }
}

// Start playing a sequence of tones
void playToneSequence(const Tone* tones, uint8_t count) {
    if (buzzerState.active) {
        // Already playing, ignore new request
        return;
    }

    if (count == 0 || count > 4) return;

    buzzerState.queueSize = count;
    buzzerState.queueIndex = 0;

    for (uint8_t i = 0; i < count; i++) {
        buzzerState.queue[i] = tones[i];
    }

    buzzerState.currentTone = buzzerState.queue[0];
    buzzerState.toneStartTime = millis();
    buzzerState.active = true;
    
    ledcAttachPin(BUZZER_PIN, 0);
    ledcWriteTone(0, buzzerState.currentTone.frequency);
    ledcWrite(0, currentVolume);  // Apply configurable volume
}

void playSound(SoundType type) {
    if (!settingsPtr || !settingsPtr->soundEnabled) return;

    Tone tones[5];
    uint8_t count = 0;

    switch (type) {
        case SoundType::WORK_END:
            // Triumphant fanfare: C5-E5-G5-C6 (523→659→784→1047 Hz)
            tones[0] = {523, 120};
            tones[1] = {659, 120};
            tones[2] = {784, 120};
            tones[3] = {1047, 350};
            count = 4;
            break;

        case SoundType::BREAK_END:
            // Gentle descending: G5-E5-C5-G4 (784→659→523→392 Hz)
            tones[0] = {784, 150};
            tones[1] = {659, 150};
            tones[2] = {523, 150};
            tones[3] = {392, 300};
            count = 4;
            break;

        case SoundType::CHIME:
            // Nice bell chime: E5-G5-B5
            tones[0] = {659, 100};
            tones[1] = {784, 120};
            tones[2] = {988, 180};
            count = 3;
            break;

        case SoundType::MODE_SWITCH:
            // Quick blip for mode cycling: D5 short
            tones[0] = {587, 60};
            count = 1;
            break;

        case SoundType::RESET_SOUND:
            // Reset sound: double beep
            tones[0] = {523, 80};
            tones[1] = {523, 80};
            count = 2;
            break;

        case SoundType::COUNTDOWN:
        default:
            // Short tick
            tones[0] = {880, 50};
            count = 1;
            break;
    }

    playToneSequence(tones, count);
}

// Play sound when timer starts (not on completion)
// - Work starts (break ended) -> play break end sound (descending, gentle)
// - Break starts (work ended) -> play work end sound (ascending, triumphant)
void playTimerStartSound(TimerMode mode, const Settings& settings) {
    if (!settings.soundEnabled) return;

    switch (mode) {
        case TimerMode::WORK:
            // Work starts = break just ended -> gentle descending sound
            playSound(SoundType::BREAK_END);
            break;
        case TimerMode::BREAK:
        case TimerMode::LONG_BREAK:
            // Break starts = work just ended -> triumphant ascending sound
            playSound(SoundType::WORK_END);
            break;
        default:
            break;
    }
}

void playChime() {
    if (!settingsPtr || !settingsPtr->soundEnabled) return;
    playSound(SoundType::CHIME);
}

void playModeSwitchSound() {
    if (!settingsPtr || !settingsPtr->soundEnabled) return;
    playSound(SoundType::MODE_SWITCH);
}

void playResetSound() {
    if (!settingsPtr || !settingsPtr->soundEnabled) return;
    playSound(SoundType::RESET_SOUND);
}

void playCountdownBeep(int remaining) {
    if (!settingsPtr || !settingsPtr->soundEnabled) return;

    if (remaining > 0) {
        // Short tick for countdown
        Tone tick = {880, 50};
        playToneSequence(&tick, 1);
    } else {
        // Final beep (double)
        const Tone finalBeep[2] = {{880, 100}, {880, 200}};
        playToneSequence(finalBeep, 2);
    }
}

bool isBuzzerActive() {
    return buzzerState.active;
}
