#pragma once
#include <filesystem>
#include "CartridgeHeader.h"



class Cartridge
{
public:
	Cartridge(CartridgeInfo&& info, std::vector<uint8_t>&& romData);
	virtual ~Cartridge();

	// Core
	virtual uint8_t Read(Address address) = 0;
	virtual void Write(Address address, uint8_t val) = 0;

	// Save files
	void LoadRAM(std::vector<uint8_t>&& ramData);
	void LoadRAMFromFilepath(const std::string& filepath);
	std::vector<uint8_t>& GetRAM() { return ram; }
	void ClearRAMDirtyFlag() { ramDirty = false; }
	bool IsRAMDirty() { return ramDirty; }
	void DumpRAMToFile();
	

	// I love cheating
	virtual void SaveState(std::ofstream& out);
	virtual void LoadState(std::ifstream& in);


	// Initialization
	static std::unique_ptr<Cartridge> CreateCartridge(std::vector<uint8_t>&& romData, std::string filepath = {});
	const CartridgeInfo* GetInfo() { return &info; }


private:
	static CartridgeInfo ParseCartridgeHeader(const std::vector<uint8_t>& romData);



protected:
	CartridgeInfo info;
	std::vector<uint8_t> rom;
	std::vector<uint8_t> ram;
	bool ramDirty = false;
};
