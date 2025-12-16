#pragma once
#include "PPU.h"

#include <array>


class PPU
{

private:
	std::array<uint8_t, 8192> vram;
	std::array<uint8_t, 160> oam;
};