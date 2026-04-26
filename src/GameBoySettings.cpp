#include "GameBoySettings.h"

const int GBSettings::CYCLES_PER_FRAME = 70224;
float GBSettings::TARGET_FRAME_TIME = 1000.0f / 59.7f;
float GBSettings::RUNTIME_SPEED = 1.0f;

bool GBSettings::ENABLE_AUDIO_CHANNEL_1 = true;
bool GBSettings::ENABLE_AUDIO_CHANNEL_2 = true;
bool GBSettings::ENABLE_AUDIO_CHANNEL_3 = true;
bool GBSettings::ENABLE_AUDIO_CHANNEL_4 = true;