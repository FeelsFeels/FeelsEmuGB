#pragma once
#include "Cartridge.h"

#include <memory>
#include <string>

class Timer;
class CPU;
class PPU;
class Joypad;

// Think of this as the motherboard
class Bus
{
public:
	Bus() = default;
	~Bus() = default;

	uint8_t Read(Address addr);
	void Write(Address addr, uint8_t data);
	void DMATransfer(uint8_t data);
	void RequestInterrupt(InterruptCode bit);


	void AttachCartridge(Cartridge* cart) 
	{ 
		debugString.clear(); cartridge = cart; 
		std::fill(wram.begin(), wram.end(), 0); 
		std::fill(hram.begin(), hram.end(), 0);
		std::fill(io.begin(), io.end(), 0);
	}
	void RemoveCartridge() { cartridge = nullptr; }

	void AttachCPU(CPU* p) { cpu = p; }
	void AttachPPU(PPU* p) { ppu = p; }
	void AttachTimer(Timer* p) { timer = p; }
	void AttachJoypad(Joypad* p) { joypad = p; }

	void RunBootRom();

	void SaveState(std::ofstream& out);
	void LoadState(std::ifstream& in);

private:
	Cartridge* cartridge;
	CPU* cpu;
	PPU* ppu;
	Timer* timer;
	Joypad* joypad;

	bool bootRomEnabled = false;	// TODO: does nothing right now. I never set it true anywhere, nor do any real checks with it.

	std::array<uint8_t, 8192> wram;
	std::array<uint8_t, 127> hram;
	std::array<uint8_t, 128> io;	// TODO: this is temporary. this is memory for the components i have not yet implemented.

	std::string debugString{};
};