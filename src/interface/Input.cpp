#include <algorithm>

#include <SDL.h>

#include "Input.h"
#include "../core/Joypad.h"
#include "../GameBoySettings.h"

const InputState& SDLInputProvider::Poll()
{
    inputState.fill(ButtonState{});
    
    SDL_PumpEvents();

    const Uint8* state = SDL_GetKeyboardState(NULL);

    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
    {
        bool current = state[i];

        inputState[i].pressed = current && !previousState[i].down;
        inputState[i].released = !current && previousState[i].down;
        inputState[i].down = current;
    }

    previousState = inputState;
    return inputState;
}

const InputState& SDLInputProvider::GetInputState() const
{
    return inputState;
}

GameInput SDLInputProvider::GetGameInput()
{
    GameInput gameInput{};

    for (auto& [gameKey, SDLKey] : GBSettings::KEY_MAPPING)
    {
        gameInput[static_cast<size_t>(gameKey)] = inputState[SDLKey];
    }
    
    return gameInput;
}
