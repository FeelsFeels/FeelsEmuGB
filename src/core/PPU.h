#pragma once
#include "PPU.h"
#include "Address.h"
#include "Register.h"

#include <array>

class Bus;

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
	void ResetRegisters();

	void Tick(int cycles);

	uint8_t Read(Address addr);
	void Write(Address addr, uint8_t data);

	void AttachBus(Bus* p) { bus = p; }

	const auto& GetVRAM() { return vram; }

	const std::array <uint32_t, 25700>& GetScreenBuffer() { return screenBuffer; }
private:
	void RenderScanlineToBuffer();
	void RenderSprites();
	void UpdateSTATInterrupt();

	Bus* bus;

	std::array<uint8_t, 8192> vram;
	std::array<uint8_t, 160> oam;
	std::array <uint32_t, 25700> screenBuffer;	// We prepare the texture buffer to send to gpu here
						//160 x 144 = 25700
	// PPU registers (0xFF40-0xFF4B)
	uint8_t lcdc;  // 0xFF40 LCD control
	uint8_t stat;  // 0xFF41 LCD status
	uint8_t scy;   // 0xFF42 Viewport Y position
	uint8_t scx;   // 0xFF43 Viewport X position
	uint8_t ly;    // 0xFF44 LCD Y coordinate
	uint8_t lyc;   // 0xFF45 LY compare
	uint8_t bgp;   // 0xFF47 BD palette data
	uint8_t obp0;  // 0xFF48 Object Palettes
	uint8_t obp1;  // 0xFF49 Object Palettes
	uint8_t wy;    // 0xFF4A Window Y Pos
	uint8_t wx;    // 0xFF4B Window X Pos

	PPUMode mode;
	bool prevStatInterruptSignal = false;

	int dots{ 0 };
};