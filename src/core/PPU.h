#pragma once
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

struct OAMObject
{
	uint8_t yPos;
	uint8_t xPos;
	uint8_t tileIndex;
	uint8_t attributes;
};
struct OAMPixel
{
	uint8_t colorIndex;
	uint8_t finalColorIndex; // Post palette adjustment
	uint8_t priority;

	uint8_t cgbPaletteIndex;
	uint8_t vramBank;
	bool occupied = false; // True if a non transparent sprite pixel was written here
};

struct BGPixel
{
	uint8_t colorIndex;
	uint8_t paletteIndex;   // bits 2-0 of attribute byte
	uint8_t vramBank;       // bit 3 of attribute byte
	bool hFlip;             // bit 5
	bool vFlip;             // bit 6
	bool priority;          // bit 7
};

struct ScanlineSprites
{
	std::array<OAMObject, 10> sprites;
	uint8_t count = 0;
};


class PPU
{
public:
	void AttachBus(Bus* p) { bus = p; }
	void ResetRegisters();

	void Tick(int cycles);

	uint8_t Read(Address addr);
	void Write(Address addr, uint8_t data);
	void ChangeVramBank(bool selectBank1) { selectedVramBank1 = selectBank1 ? true : false; };
	void SetCGBMode(bool b) { cgbMode = b; }

	const std::array<uint8_t, 8192>& GetVRAM() { return vram; }
	const std::array <uint32_t, 25700>& GetScreenBuffer() { return screenBuffer; }

	void SaveState(std::ofstream& out);
	void LoadState(std::ifstream& in);

	// Editor
	uint8_t GetLCDC() const { return lcdc; }
	uint8_t GetSCY() const { return scy; }
	uint8_t GetSCX() const { return scx; }

public:
	void RenderScanlineToBuffer();
	void RenderScanlineToBuffer2();
	void UpdateSTATInterrupt();
	uint32_t CGBColorToARGB(uint8_t paletteIndex, uint8_t colorIndex, bool isObj);

	Bus* bus;

	std::array<uint8_t, 8192> vram;
	std::array<uint8_t, 8192> vram1;
	std::array<uint8_t, 160> oam;
	std::array <uint32_t, 25700> screenBuffer;	// We prepare the texture buffer to send to gpu here
												//160 x 144 = 25700
	std::array<uint8_t, 64> cgbBgPaletteData;
	std::array<uint8_t, 64> cgbObjPaletteData;

	// PPU registers (0xFF40-0xFF4B)
	uint8_t lcdc;  // 0xFF40 LCD control
	uint8_t stat;  // 0xFF41 LCD status
	uint8_t scy;   // 0xFF42 Viewport Y position
	uint8_t scx;   // 0xFF43 Viewport X position
	uint8_t ly;    // 0xFF44 LCD Y coordinate (Which line the PPU is drawing to)
	uint8_t lyc;   // 0xFF45 LY compare
	uint8_t bgp;   // 0xFF47 BD palette data (DMG only)
	uint8_t obp0;  // 0xFF48 Object Palettes
	uint8_t obp1;  // 0xFF49 Object Palettes
	uint8_t wy;    // 0xFF4A Window Y Pos
	uint8_t wx;    // 0xFF4B Window X Pos

	// CGB Registers
	uint8_t bcps;  // 0xFF68 Background color palette specification
	uint8_t bcpd;  // 0xFF69 Background color palette data
	uint8_t ocps;  // 0xFF6A Object color palette specification
	uint8_t ocpd;  // 0xFF6B Background color palette data
	uint8_t opri;  // 0xFF6C Object priority mode


	bool windowYCondition = false;
	uint8_t windowRowsDrawn = 0;

	PPUMode mode;
	bool prevStatInterruptSignal = false;
	
	bool cgbMode = false;
	bool selectedVramBank1 = false;



	int dots{ 0 };

	// Internal drawing resolution
	std::array<BGPixel, 160> bgPixels;
	std::array<OAMPixel, 160> spritePixels;

	std::array<uint8_t, 160> bgColorIndex;
	std::array<uint8_t, 160> bgPriority;

	ScanlineSprites scanlineSprites;

};