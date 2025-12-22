#pragma once
#include <cstdint>
enum class InterruptCode : uint8_t
{
	VBLANK = 0,
	LCD = 1,
	TIMER = 2,
	SERIAL = 3,
	JOYPAD = 4
};