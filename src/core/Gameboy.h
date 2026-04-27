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

	bool HasCartridge() const { return cart != nullptr; }

	const CartridgeInfo* GetCartInfo() const;
	const std::string& GetCartPath() const { return pathToCartridge; };

	int Update();
	const std::array <uint32_t, 25700>& GetScreenBuffer() { return ppu.GetScreenBuffer(); }

	void SaveState();
	void LoadState();

	void UpdateInput(IInputProvider& input);

	void SetAudioSampleRate(float sampleRate);
	std::vector<float>& GetAudioBuffer() { return apu.GetAudioBuffer(); }
	void ClearAudioBuffer() { apu.ClearAudioBuffer(); }

	void GetChannelVolumes(float& ch1, float& ch2, float& ch3, float& ch4);
private:

	void SaveGame();

	friend class DebugInfo;
	friend class VRAMBrowser;
	friend class TileMapBrowser;

	std::string pathToCartridge;
	std::shared_ptr<Cartridge> cart;

	Bus bus;
	CPU cpu;
	PPU ppu;
	APU apu;
	Timer timer;
	Joypad joypad;
};
