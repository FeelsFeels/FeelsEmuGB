#include "GameBoySettings.h"

const int GBSettings::CYCLES_PER_FRAME = 70224;
float GBSettings::TARGET_FRAME_TIME = 1000.0f / 59.7f;
float GBSettings::RUNTIME_SPEED = 1.0f;

bool GBSettings::ENABLE_AUDIO_CHANNEL_1 = true;
bool GBSettings::ENABLE_AUDIO_CHANNEL_2 = true;
bool GBSettings::ENABLE_AUDIO_CHANNEL_3 = true;
bool GBSettings::ENABLE_AUDIO_CHANNEL_4 = true;


std::unordered_map<Buttons, int> GBSettings::KEY_MAPPING =
{
    {Buttons::A,      SDL_SCANCODE_K},
    {Buttons::B,      SDL_SCANCODE_J},
    {Buttons::UP,     SDL_SCANCODE_W},
    {Buttons::DOWN,   SDL_SCANCODE_S},
    {Buttons::LEFT,   SDL_SCANCODE_A},
    {Buttons::RIGHT,  SDL_SCANCODE_D},
    {Buttons::START,  SDL_SCANCODE_X},
    {Buttons::SELECT, SDL_SCANCODE_Z}
};