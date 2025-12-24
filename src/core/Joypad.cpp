#include "Joypad.h"

//TODO: Joypad interrupts
void Joypad::Reset()
{
	up = false; down = false; left = false; right = false;
	a = false; b = false;
	start = false; select = false;
	reg = 0xCF;
}

void Joypad::ButtonDown(Buttons button)
{
	switch (button)
	{
	case Buttons::A:      a = true; break;
	case Buttons::B:      b = true; break;
	case Buttons::UP:     up = true; break;
	case Buttons::DOWN:   down = true; break;
	case Buttons::LEFT:   left = true; break;
	case Buttons::RIGHT:  right = true; break;
	case Buttons::START:  start = true; break;
	case Buttons::SELECT: select = true; break;
	}
}

void Joypad::ButtonUp(Buttons button)
{
	switch (button)
	{
		case Buttons::A:      a = false; break;
		case Buttons::B:      b = false; break;
		case Buttons::UP:     up = false; break;
		case Buttons::DOWN:   down = false; break;
		case Buttons::LEFT:   left = false; break;
		case Buttons::RIGHT:  right = false; break;
		case Buttons::START:  start = false; break;
		case Buttons::SELECT: select = false; break;
	}
}

// 0 being pressed, 1 being NOT pressed
uint8_t Joypad::GetInput()
{
	uint8_t output = 0xC0 | (reg & 0x30);

	bool selectButtons = !(reg & 0x20); // Bit 5 is 0 = selected
	bool selectDpad    = !(reg & 0x10); // Bit 4 is 0 = selected

	uint8_t lowerNibble = 0x0F;

	if (selectButtons && selectDpad)
	{
		// UB
		return lowerNibble;
	}

	if (selectButtons)
	{
		if (a)      lowerNibble &= 0b1110;
		if (b)      lowerNibble &= 0b1101;
		if (select) lowerNibble &= 0b1011;
		if (start)  lowerNibble &= 0b0111;
	}
	if (selectDpad)
	{
		if (right) lowerNibble &= 0b1110;
		if (left)  lowerNibble &= 0b1101;
		if (up)    lowerNibble &= 0b1011;
		if (down)  lowerNibble &= 0b0111;
	}

	return output | lowerNibble;
}

void Joypad::Write(uint8_t val)
{
	reg = val & 0x30;
}


