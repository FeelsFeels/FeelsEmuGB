#include "Gameboy.h"

GameBoy::GameBoy()
{
	cpu.AttachBus(&bus);
}

GameBoy::~GameBoy()
{}

void GameBoy::InsertCartridge(std::string filepath)
{
	// Clear previous info first
	bus.RemoveCartridge();
	cart.reset();
	pathToCartridge.clear();

	std::vector<uint8_t> romData;
	if (!VFS::ReadFile(filepath, romData))
	{
		// Warn
		std::cout << "Insert Cartridge: Bad path: " << filepath << "\n";
		return;
	}
	
	std::cout << "Loaded Rom: " << filepath << "\n";
	pathToCartridge = filepath;
	cart = Cartridge::CreateCartridge(std::move(romData));
	bus.AttachCartridge(cart.get());


	//std::cout << "Title: " << cart->info.title << "\n";
	//std::cout << "Cart type: " << cart->info.cartTypeString << "\n";
	//std::cout << "RomSizes: " << cart->info.romSizeBytes << "\n";
	//std::cout << "RomBanks: " << cart->info.romBanks << "\n";
	//std::cout << "Ramsize: " << cart->info.ramSizeBytes << "\n";
	//std::cout << "RamBanks: " << cart->info.ramBanks << "\n";
	//std::cout << "HasRam: " << cart->info.hasRam << "\n";
	//std::cout << "Batt: " << cart->info.hasBattery << "\n";
	//std::cout << "Timer: " << cart->info.hasTimer << "\n";
}

const CartridgeInfo& GameBoy::GetCartInfo() const
{
	return cart->info;
}
