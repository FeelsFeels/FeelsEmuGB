#pragma once
#include <filesystem>
#include "CartridgeHeader.h"



class Cartridge
{
public:
	Cartridge(CartridgeInfo&& info, std::vector<uint8_t>&& romData);
	~Cartridge();

	virtual void Read(Address address) = 0;
	virtual void Write(Address address) = 0;

	void LoadRAM(std::vector<uint8_t> ramData);

	static std::unique_ptr<Cartridge> CreateCartridge(std::vector<uint8_t>&& romData);

private:
	static CartridgeInfo ParseCartridgeHeader(const std::vector<uint8_t>& romData);



protected:
	CartridgeInfo info;
	std::vector<uint8_t> rom;
	std::vector<uint8_t> ram;
};
