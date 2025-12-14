#include "Gameboy.h"

GameBoy::GameBoy()
{
	
}

GameBoy::~GameBoy()
{}

void GameBoy::InsertCartridge(std::string filepath)
{
	std::vector<uint8_t> romData;
	
	//CartridgeInfo cartInfo = Cartridge::ParseCartridgeHeader(romData);
	
	cart = Cartridge::CreateCartridge(std::move(romData));
}
