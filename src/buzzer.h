#pragma once

#include "config.h"
#include "types.h"

void setupBuzzer();
void updateBuzzer();
void playToneSequence(const Tone* tones, uint8_t count);
void playSound(SoundType type);
void playTimerStartSound(TimerMode mode, const Settings& settings);
void playChime();
void playModeSwitchSound();
void playResetSound();
void playCountdownBeep(int remaining);
void setBuzzerSettings(const Settings& settings);
void setBuzzerVolume(uint8_t volume);
bool isBuzzerActive();
