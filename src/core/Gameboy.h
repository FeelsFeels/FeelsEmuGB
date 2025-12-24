#pragma once
#include "Bus.h"
#include "Cartridge.h"
#include "Timer.h"
#include "CPU.h"
#include "PPU.h"
#include "Joypad.h"

#include <memory>

class GameBoy
{
public:
	GameBoy();
	~GameBoy();

	void InsertCartridge(std::string filepath);

	const CartridgeInfo& GetCartInfo() const;
	const std::string& GetCartPath() const { return pathToCartridge; };

	int Update();
	const std::array <uint32_t, 25700>& GetScreenBuffer() { return ppu.GetScreenBuffer(); }

	void UpdateInput(std::unordered_map<SDL_Scancode, ButtonState>& keyboard);

private:
	friend class DebugInfo;
	friend class VRAMBrowser;

	std::string pathToCartridge;
	std::shared_ptr<Cartridge> cart;

	Bus bus;
	CPU cpu;
	PPU ppu;
	Timer timer;
	Joypad joypad;
};
