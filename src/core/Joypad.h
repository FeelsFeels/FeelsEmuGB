#pragma once

#include <cstdint>
#include "../interface/Input.h"



class Joypad
{
public:
	void Reset();
	void ButtonDown(Buttons button);
	void ButtonUp(Buttons button);

	uint8_t GetInput();
	void Write(uint8_t val);

private:
	uint8_t reg;

	bool up, down, left, right;
	bool a, b;
	bool start, select;

	
};