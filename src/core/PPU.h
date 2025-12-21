#pragma once
#include "PPU.h"
#include "Address.h"
#include "Register.h"

#include <array>


enum PPUMode : uint8_t
{
	HBLANK   = 0,
	VBLANK   = 1,
	OAM_SCAN = 2,
	DRAWING	 = 3
};

class PPU
{
public:
	void Tick(int cycles);

	uint8_t Read(Address addr);
	void Write(Address addr, uint8_t data);

private:

	std::array<uint8_t, 8192> vram;
	std::array<uint8_t, 160> oam;

	// PPU registers (0xFF40-0xFF4B)
	uint8_t lcdc;  // 0xFF40 LCD control
	uint8_t stat;  // 0xFF41 LCD status
	uint8_t scy;   // 0xFF42 Viewport Y position
	uint8_t scx;   // 0xFF43 Viewport X position
	uint8_t ly;    // 0xFF44 LCD Y coordinate
	uint8_t lyc;   // 0xFF45 LY compare
	uint8_t bgp;   // 0xFF47 BD palette data


	PPUMode mode;

	int dots{ 0 };
};