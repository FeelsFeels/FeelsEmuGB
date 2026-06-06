#pragma once
#include "Bus.h"
#include "Cartridge.h"
#include "Timer.h"
#include "CPU.h"
#include "PPU.h"
#include "APU.h"
#include "Joypad.h"

#include <memory>

class GameBoy
{
public:
	GameBoy();
	~GameBoy();

	void InsertCartridge(std::string filepath);
	void InsertCartridge(std::vector<uint8_t>&& romData);
	void OpenSaveFile(std::vector<uint8_t>&& ramData);

	int Update();
	void UpdateInput(IInputProvider& input);

	void SaveState();
	void LoadState();

	void SetAudioSampleRate(float sampleRate);
	std::vector<float>& GetAudioBuffer() { return apu.GetAudioBuffer(); }
	void ClearAudioBuffer() { apu.ClearAudioBuffer(); }
	
	
	// Exposing of data to renderer/editor
	//uint8_t EditorBusRead(Address addr) { return bus.Read(addr); }
	const std::array <uint32_t, 25700>& GetScreenBuffer() { return ppu.GetScreenBuffer(); }
	const std::array<uint8_t, 8192>& GetVRAM() { return ppu.GetVRAM(); }
	uint8_t GetLCDC() { return ppu.GetLCDC(); }
	uint8_t GetSCY() { return ppu.GetSCY(); }
	uint8_t GetSCX() { return ppu.GetSCX(); }

	void GetChannelVolumes(float& ch1, float& ch2, float& ch3, float& ch4);

	bool HasCartridge() const { return cart != nullptr; }
	const CartridgeInfo* GetCartInfo() const;
	const std::string& GetCartPath() const { return pathToCartridge; };

	const Registers& GetCPURegisters() { return cpu.GetRegisters(); }
	CPU& GetCPU() { return cpu; }

private:

	void SaveGame();

	std::string pathToCartridge;
	std::shared_ptr<Cartridge> cart;

	Bus bus;
	CPU cpu;
	PPU ppu;
	APU apu;
	Timer timer;
	Joypad joypad;

	bool cgbMode = false;
};
