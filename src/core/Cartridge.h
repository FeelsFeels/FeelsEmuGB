#pragma once
#include <filesystem>
#include "CartridgeHeader.h"



class Cartridge
{
public:
	Cartridge(CartridgeInfo&& info, std::vector<uint8_t>&& romData);
	~Cartridge();

	virtual uint8_t Read(Address address) = 0;
	virtual void Write(Address address, uint8_t val) = 0;

	void LoadRAM(std::vector<uint8_t> ramData);

	static std::unique_ptr<Cartridge> CreateCartridge(std::vector<uint8_t>&& romData);

	CartridgeInfo info;
private:
	static CartridgeInfo ParseCartridgeHeader(const std::vector<uint8_t>& romData);



protected:
	std::vector<uint8_t> rom;
	std::vector<uint8_t> ram;
};
