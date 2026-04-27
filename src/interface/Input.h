#pragma once
#include <array>
#include <unordered_map>

enum class Buttons : uint8_t
{
	A = 0,
	B = 1,
	UP = 2,
	DOWN = 3,
	LEFT = 4,
	RIGHT = 5,
	START = 6,
	SELECT = 7,
	COUNT = 8
};

struct ButtonState
{
	bool down = false;
	bool pressed = false;
	bool released = false;
};

using InputState = std::array<ButtonState, 512>;
using GameInput = std::array<ButtonState, static_cast<size_t>(Buttons::COUNT)>;


class IInputProvider
{
public:
    virtual ~IInputProvider() = default;

    virtual const InputState& Poll() = 0;
    virtual const InputState& GetInputState() const  = 0;
	virtual GameInput GetGameInput() = 0;
};

class SDLInputProvider : public IInputProvider
{
public:
    const InputState& Poll() override;
    const InputState& GetInputState() const;
	GameInput GetGameInput();

private:
    InputState inputState{};
    InputState previousState{};
};

