#include "buzzer.h"

static BuzzerState buzzerState;
static const Settings* settingsPtr = nullptr;

void setupBuzzer() {
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
}

void setBuzzerSettings(const Settings& settings) {
    settingsPtr = &settings;
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
}

void playSound(SoundType type) {
    if (!settingsPtr || !settingsPtr->soundEnabled) return;
    
    Tone tones[4];
    uint8_t count = 0;
    
    switch (type) {
        case SoundType::WORK_END:
            // Ascending C5-E5-G5 (523→659→784 Hz) - triumphant major chord
            tones[0] = {523, 250};
            tones[1] = {659, 250};
            tones[2] = {784, 280};
            count = 3;
            break;
            
        case SoundType::BREAK_END:
            // Descending G5-E5-C5 (784→659→523 Hz) - gentle descending
            tones[0] = {784, 250};
            tones[1] = {659, 250};
            tones[2] = {523, 280};
            count = 3;
            break;
            
        case SoundType::CHIME:
            // Pleasant chime
            tones[0] = {523, 150};
            tones[1] = {659, 150};
            tones[2] = {784, 200};
            count = 3;
            break;
            
        case SoundType::COUNTDOWN:
        default:
            // Simple beep
            tones[0] = {880, 100};
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

void playCountdownBeep(int remaining) {
    if (!settingsPtr || !settingsPtr->soundEnabled) return;
    
    if (remaining > 0) {
        // Short tick for countdown
        Tone tick = {880, 50};
        playToneSequence(&tick, 1);
    } else {
        // Final beep (double)
        Tone finalBeep[2] = {{880, 100}, {880, 200}};
        playToneSequence(finalBeep, 2);
    }
}

bool isBuzzerActive() {
    return buzzerState.active;
}
